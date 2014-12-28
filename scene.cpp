#include "scene.h"
#include "griditem.h"
//#include "Circle.h"
#include "DrawingItem.h"
#include <QGraphicsScene>
#include <QPointF>

Scene::Scene(QObject *parent)
        :QGraphicsScene(parent)
{
	myGrid = new GridItem(0);
	myGrid->setSpacing(2);
	addItem(myGrid);

    // create test circles:
//    for(int n=0; n<100; n++)
//    {
//		testCircle = new Circle((QGraphicsItem *)0,this);
//		testCircle->addPoint(QPointF(-1000+n*5,-500)); // center
//		testCircle->addPoint(QPointF(-1000+n*5,-450)); // "radius"
//		addItem(testCircle);
//    }
}

GridItem* Scene::grid() const{
	return myGrid;
}

QPointF Scene::snapToGrid(QPointF p) const
{
	QPointF pG; // point on grid
	qreal spacing = myGrid->spacing();

	pG.setX(spacing*qRound(p.x()/spacing));
	pG.setY(spacing*qRound(p.y()/spacing));

	return pG;
}


// added: 29.12.2009
//void Scene::drawBackground(QPainter * painter, const QRectF & rect )
//{
//	// background does not work like expected.
//	painter->setBackground(Qt::black);
//	painter->setBackgroundMode(Qt::OpaqueMode);
//
//    painter->setPen(Qt::darkCyan);
//	// TODO: improve paint performance!
//    for(qreal y=rect.y(); y<=rect.height(); y+=myGrid->spacing()*10){
//        for(qreal x=rect.x(); x<rect.width(); x+=myGrid->spacing()*10){
//            painter->drawPoint(QPointF(x,y));
//        }
//    }
//}

