
/*
 * cutout2.h
 *
 *  Created on: 18.01.2012
 *      Author: Karl Zeilhofer
 */

#ifndef ROTATEDRECT_h
#define ROTATEDRECT_h

#include <QGraphicsItem>
#include <QPoint>
#include <QPainter>
#include "DrawingItem.h"
#include <QStyleOptionGraphicsItem> // needed in painter method for setClipRect()
#include <QPainterPath>

/*
 * Grafik-Objekt: Rechteck
 * Definiert durch 4 Eckpunkte
 * Größe, Position und Rotation werden approximiert.
 */

class RotatedRect : public DrawingItem
{
public:
    RotatedRect(QGraphicsItem *parent, QGraphicsScene *scene) :
        DrawingItem(parent, scene) { // automatischer Aufruf des übergeordneten-Konstruktors
        neededPoints = 4;
        type = RotatedRect::Page; // default

        setPos(QPointF(0, 0));
        setFlag(QGraphicsItem::ItemIsMovable, false);
        setFlag(QGraphicsItem::ItemIsSelectable, true);
    };

    // TODO: Snap Item to Grid on Move per Drag'n'Drop (mouseMoveEvent() and mousePressEvent())

    QList<QPointF> allInputPoints(bool* ok = 0); // in scene coordinates
    QRectF getRectInItem(); // returns the drawn rect in item-coordinates.
    QPointF originInScene(); // returns bottom left corner of the rect in scene coordinates
    QPolygonF getPolygonInScene(); // returns the rectangle, represented as a QPolygonF in Scene-coordinates

    // zu überladende funktionen
    // schlüsselwort "virtual" ist nicht notwendig, aber besser lesbar.
    virtual int addPoint(QPointF p);
    virtual void updateLastPoint(QPointF p);
    virtual int pointsLeft() {
        return neededPoints - points.size();
    };


    // überladen:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget);

    QPainterPath shape() const;
protected:
    // überladen:
    virtual QRectF boundingRect() const;


private:
//	void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
//	void mousePressEvent(QGraphicsSceneMouseEvent *event);
//	void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    qreal getInclination(QPointF p);

public:
    enum Type{Page, Thumb} type; // this class is used for page-marking and thumb-marking
    qreal degrees;

private:
    //QPoint p1; // is set via x,y inherited from QGraphicsItem
    // QPointF p2,p3,p4;
    qreal cX, cY, w, h, phi; // center, size, rotation of the estemated rect
    QList<QPointF> rectPoints;
};

#endif // ROTATEDRECT_h
