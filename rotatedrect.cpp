#include "rotatedrect.h"

/*
 * Rectangle.cpp
 *
 *  Created on: 12.01.2010
 *      Author: Karl
 */

//#include "qwire.h"
#include <cmath>
#include "view.h"
#include <QGraphicsSceneMouseEvent>


/*
 * define a rectangular, rotated bounding rect, which will be transformed and exported.
 * Step 1: draw a horizontal line (e.g. alignment to a row of text, or the edge of a figure)
 *      the line has to have the final width
 * Step 2: set top bounding
 * Step 3: set bootm bounding
 * oprional Step 4: set left bounding, skip this step with ESC
 * optional Step 5: set right bounding, skip this step with ESC
*/

QList<QPointF> RotatedRect::allInputPoints(bool *ok)
{
    QList<QPointF> list;
    if(points.size()==neededPoints){
        if(ok)
            *ok = true;
    }else{
        if(ok)
            *ok = false;
        return list;
    }

    list.append(mapToScene(points.at(0)));
    list.append(mapToScene(points.at(1)));
    list.append(mapToScene(points.at(2)));
    list.append(mapToScene(points.at(3)));

    return list;
}

// it is translated, so that the bottom-left corner is at (0/0)
// this point can be retrieved with originInScene()
QRectF RotatedRect::getRectInItem()
{
    QRectF r = boundingRect();
    r.translate(-r.bottomLeft());
    return r;
}

QPointF RotatedRect::originInScene()
{
    return mapToScene(boundingRect().bottomLeft());
}

QPolygonF RotatedRect::getPolygonInScene()
{
    return mapToScene(boundingRect());
}

int RotatedRect::addPoint(QPointF p) {
    // set position of the item with the initial point.
    // this is important for the default-implementation of rubberband-selection, because
    // it assumes, that the point (0,0) is within the shape of the item!
    if(points.size()==0){
        setPos(p.x(), p.y());
    }

    p = mapFromScene(p); // tranform into item coordinates

    if (neededPoints - points.size() > 0) {
        points.append(p);
    }

    // if we have only 2 points, the item isn't rotated.

    if(points.size() == 3){
        QPointF cor = points.first(); // center of rotation
        QPointF AB = points.at(1)-points.at(0);
        degrees = getInclination(AB)*360/(2*M_PI);
        QTransform t = QTransform().translate(cor.x(), cor.y()).rotate(degrees).translate(-cor.x(), -cor.y());
        this->setTransform(t); // rotate item

        // rotate back the first 3 points!
        points.replace(0, t.inverted().map(points.at(0)));
        points.replace(1, t.inverted().map(points.at(1)));
        points.replace(2, t.inverted().map(points.at(2)));
    }

    prepareGeometryChange();
    return neededPoints - points.size();
}

void RotatedRect::updateLastPoint(QPointF p) {
    p = mapFromScene(p); // tranform into item coordinates

    if (points.size() > 0) {
        points.replace(points.size()-1, p);
    }
    setSelected(false);
    prepareGeometryChange();
}

QPainterPath RotatedRect::shape() const {
    QPainterPath path;
    path.moveTo(boundingRect().topLeft());
    path.addRect(boundingRect());
    return path;
}

QRectF RotatedRect::boundingRect() const
{
    switch(points.size()){
    case 1:{
        QPointF A=points.at(0);
        return QRectF(A.x()-0.5, A.y()-0.5,
                      +1, +1);
    }break;
    case 2:{
        QPointF A=points.at(0);
        QPointF B=points.at(1);
        return QRectF(fmin(A.x(), B.x())-0.5, fmin(A.y(), B.y())-0.5,
                      fabs(A.x()-B.x())+1, fabs(A.y()-B.y())+1);
    }break;
    case 3:{
        QPointF A=points.at(0);
        QPointF B=points.at(1);
        QPointF C=points.at(2);
        return QRectF(fmin(A.x(), B.x())-0.5, fmin(C.y(), A.y())-0.5,
                      fabs(A.x()-B.x())+1, fabs(C.y()-A.y())+1);
    }break;
    case 4:{
        QPointF A=points.at(0);
        QPointF B=points.at(1);
        QPointF C=points.at(2);
        QPointF D=points.at(3);
        return QRectF(fmin(A.x(), B.x())-0.5, fmin(C.y(), D.y())-0.5,
                      fabs(A.x()-B.x())+1, fabs(C.y()-D.y())+1);
    }break;
    default:
        return QRectF();
    }
}


void RotatedRect::paint(QPainter *painter,
                        const QStyleOptionGraphicsItem *option, QWidget *widget) {

    // CMYK Pelette füllt das Rechteck
    // CMYK hat 4 unabhängige Variablen. Leider können nur 2 davon variiert werden,
    // damit eine Fläche gezeichnet werden kann.

    painter->setPen(Qt::blue);

    QPen penPage(QColor(20,200,20,255));
    QPen penThumb(QColor(200,20,20,255));
    QPen penText(QColor(0,0,0,40));
    QBrush brushPage(QColor(20,200,20,30));
    QBrush brushThumb(QColor(200,20,20,30));



    QPainterPath cross;
    cross.moveTo(QPointF(-10,0));
    cross.lineTo(QPointF(10,0));
    cross.moveTo(QPointF(0,-10));
    cross.lineTo(QPointF(0,10));

//    painter->drawRect(0,0,100,100); //just for debugging.
//    if(points.size()>0) // just for debugging
//        painter->drawPath(cross.translated(points.last()));

    if(type == Page){
        painter->setPen(penPage);
        painter->setBrush(brushPage);
    }else{
        painter->setPen(penThumb);
        painter->setBrush(brushThumb);
    }
    if(points.size()==1){
        QPointF A=points.at(0);
        //painter->drawText(A.x()+10, A.y()+10, "Cutout2");
    }else if(points.size()==2){
        QPainterPath path;
        QPointF A=points.at(0);
        QPointF B=points.at(1);
        QPointF AB = B-A;
        QPointF ABn = QPointF(-AB.y(), AB.x()); // normal vector

        painter->drawLine(A,B);

        // draw H-formed lines, to easy the imagination of the to be defined rectangle
        painter->drawLine(A, A+2*ABn);
        painter->drawLine(A, A-2*ABn);
        painter->drawLine(B, B+2*ABn);
        painter->drawLine(B, B-2*ABn);
    }else if(points.size()==3){
        QPointF A=points.at(0);
        QPointF B=points.at(1);
        QPointF C=points.at(2);
        painter->drawRect(A.x(), A.y(), B.x()-A.x(), C.y()-A.y());
    }else if(points.size()==4){
        QPointF A=points.at(0);
        QPointF B=points.at(1);
        QPointF C=points.at(2);
        QPointF D=points.at(3);


        QRectF r = boundingRect();
        painter->drawRect(r);

        // draw text (name):
        painter->setPen(penText);

        QString str = name;
        QPoint pos( r.x()+r.width()/2, r.y() + r.height()/2);
        pos = r.bottomLeft().toPoint();
        pos.setX(pos.x() + r.width()/2);
        pos.setY(pos.y() - r.height()/2);

        QFont font;
        font.setPointSize( 40 );

        QFontMetrics fm( font );
        QRect br( pos, fm.size( 0, str ) );
        qreal scaleW = r.width()/br.width();
        qreal scaleH = r.height()/br.height();
        font.setPointSize(40*fmin(scaleW, scaleH)*0.9); // scale to 90% of the width of the rect.
        fm = QFontMetrics(font);
        br= QRect( pos, fm.size( 0, str ) );
        pos.setX(pos.x() - br.width()/2);
        pos.setY(pos.y() + br.height()/4);

        painter->setFont( font );
        painter->drawText(pos, str);
    }

    if (isSelected()) {
        painter->setPen(QPen(Qt::blue, 4, Qt::DashLine, Qt::RoundCap,
                             Qt::RoundJoin));
        QRectF r = boundingRect();
        r.adjust(-2,-2,2,2); // expand boundingRect by 2 units
        painter->drawRect(r);
    }

}



// returns angle to horizontal or vertical in radians
// value is between -pi/4 to +pi/4°
qreal RotatedRect::getInclination(QPointF p)
{
    qreal x = p.x();
    qreal y = p.y();
    qreal alpha;

//	if(x == 0 || y == 0){
//		return 0.0;
//	}

//	if(fabs(y/x) <= 1){
//		alpha = atan(y/x);
//	}else
//	{
//		alpha = -atan(x/y);
//	}

    return atan2(y,x);

    return alpha;
}


