#include "view.h"
#include "crosshair.h"
#include "scene.h"

#include <QtGui>
#include <QGraphicsScene>
#include <QScrollBar>
#include <QWheelEvent>
#include <QtOpenGL>
#include <QObject>
#include "DrawingItem.h"
#include "rotatedrect.h"
#include "crosshair.h"
#include <QPaintEvent>


View::View(QWidget* parent):QGraphicsView(parent)
{
	setRenderHints(QPainter::Antialiasing);
    setViewport(new QGLWidget(QGLFormat(QGL::DoubleBuffer))); // use OpenGL // ursprünglich SampleBuffers
	setMouseTracking(true); // enable mouse tracking for positioning the cross hair
    //setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    setDragMode(QGraphicsView::RubberBandDrag); // enable selecting items by "rubber band"
    //setCursor(Qt::BlankCursor); // set curosr invisible
    setBackgroundBrush(QBrush(Qt::black, Qt::SolidPattern));

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    currentTool = Tool_None;

    scene = new Scene(this); // TODO: manage a typecast of this (View*) to QObject* and pass this as argument

    crossHair = new CrossHair(NULL,NULL);


	scene->addItem(crossHair);

	qreal s = 20000; // size of the scene square rect
	scene->setSceneRect(-s/2,-s/2,s,s);

    setScene(scene);
	//connect(this, SIGNAL(scaleChanged(qreal)),scene->grid(), SLOT(setViewScale(qreal)));

    drawingState=0; // Zustand für Grafik-Objekte zeichnen: idle
    newItem = 0;
}

/*
 * handles zooming of the view.
 * keeps the same scene-point under the cursor while zooming in and out
 */
void View::wheelEvent(QWheelEvent *event)
{
    int degrees = event->delta()/8;

    if(degrees > 0) // wheel from user away (zoom in)
    {
        if(matrix().m11() < 100) // limit zoom factor
        {
            scale(zoomFactor, zoomFactor);
            // scroll values:
            int sX = (scene->snapToGrid(event->pos()).x() - viewport()->width()/2)*(zoomFactor - 1);
            int sY = (scene->snapToGrid(event->pos()).y() - viewport()->height()/2)*(zoomFactor - 1);
            horizontalScrollBar()->setValue(horizontalScrollBar()->value()+sX);
            verticalScrollBar()->setValue(verticalScrollBar()->value()+sY);
        }
    }
    if(degrees < 0) // wheel turned to user (zoom out)
    {
		if(matrix().m11() > 1./20.) // limit zoom factor
        {
            scale((qreal)(1./zoomFactor), (qreal)(1./zoomFactor));
            // scroll values:
            int sX = (scene->snapToGrid(event->pos()).x() - viewport()->width()/2)*(1.0/zoomFactor - 1);
            int sY = (scene->snapToGrid(event->pos()).y() - viewport()->height()/2)*(1.0/zoomFactor - 1);
            horizontalScrollBar()->setValue(horizontalScrollBar()->value()+sX);
            verticalScrollBar()->setValue(verticalScrollBar()->value()+sY);
        }
    }
    emit scaleChanged((qreal)(matrix().m11()));
}

// TODO: implement panning with middle-mouse button.
void View::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton){
        switch(drawingState)
        {
        case 1: // add first point, when not already happened in mouse-move-event
            newItem->addPoint(scene->snapToGrid(mapToScene(event->pos()))); // add first point
            break;
        case 2:// add additional points
            newItem->updateLastPoint(scene->snapToGrid(mapToScene(event->pos())));
            newItem->addPoint(scene->snapToGrid(mapToScene(event->pos())));
            update();
            if(newItem->pointsLeft()<=0){
                drawingState=3;
            }
            // scene->addItem(newItem); // is already done in the constructor of the item
            break;
        case 3: // set last point
        {
            newItem->updateLastPoint(scene->snapToGrid(mapToScene(event->pos()))); // fix recent point
            drawingState=0; // Element ist fertig --> idle
            DrawingItem* item = newItem;
            newItem=0; // set to NULL, otherwise it gets deleted!


            switch(currentTool)
            {
            case Tool_Page:
            case Tool_Thumb:
                emit finishedRotatedRect((RotatedRect*)item);
                break;
            }

        }break;
        default:
            drawingState=0; // just for savety
            newItem =0;
        }
    }
    QGraphicsView::mousePressEvent(event); // call standard implementation
}


void View::mouseMoveEvent(QMouseEvent *event)
{
    if(drawingState == 1){
        newItem->addPoint(scene->snapToGrid(mapToScene(event->pos())));
        drawingState = 2;
    }else if(drawingState == 2 || drawingState == 3){ // beziet sich auf Element Erstellung.
		newItem->updateLastPoint(scene->snapToGrid(mapToScene(event->pos())));
    }

    crossHair->setPos(scene->snapToGrid(mapToScene(event->pos())));
    QGraphicsView::mouseMoveEvent(event); // call standard implementation
    update();
}

// slot:
//void View::newCircle()
//{
//	drawingState=1; // wait for first point input
//	newItem=new Circle((QGraphicsItem*)0,scene);
//}

//void View::newRectangle()
//{
//	drawingState=1; // wait for first point input
//	newItem=new MyRectangle((QGraphicsItem*)0,scene);
//}

void View::newRotatedRect(RotatedRect::Type type)
{
    drawingState=1; // wait for first point input
    if(newItem!=0){
        newItem->~QGraphicsItem(); // delete unfinished items!
    }
    newItem=new RotatedRect((QGraphicsItem*)0,scene);
    ((RotatedRect*)newItem)->type = type;
    if(type == RotatedRect::Page){
        currentTool = Tool_Page;
    }else if(type == RotatedRect::Thumb){
        currentTool = Tool_Thumb;
    }
}

Scene* View::getScene() const
{
    return scene;
}

void View::setVisibleRectangle(QRectF sceneRect)
{
    double zoomFactor;

    if((double)width()/(double)height() >
            (double)sceneRect.width()/(double)sceneRect.height()){
        qDebug()<<"fit to height";
        zoomFactor = (double)height()/sceneRect.height();
    }else{
        qDebug()<<"fit to width";
        zoomFactor = (double)width()/sceneRect.width();
    }

    qDebug()<<"zoomFactor = " << zoomFactor;

    setMatrix(QMatrix().scale(zoomFactor,zoomFactor));
    horizontalScrollBar()->setValue(sceneRect.topLeft().x()*zoomFactor);
    verticalScrollBar()->setValue(sceneRect.topLeft().y()*zoomFactor);

    emit scaleChanged((qreal)(matrix().m11()));
}

void View::stopCurrentTool()
{
    if(newItem){
        newItem->~DrawingItem();
        newItem=0;
        repaint();
    }
    drawingState = 0;
}

