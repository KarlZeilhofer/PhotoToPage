#ifndef SCENE_H
#define SCENE_H

#include <QGraphicsScene>
//#include "Circle.h"
#include "DrawingItem.h"

class GridItem;

/*
 * Scene st vorläufig eine Klasse, die einer Zeichnung entspricht.
 * Sie beinhaltet alle Zeichenobjekte und besitzt ein vom Bildschirm unabhängiges
 * Koordinatensystem.
 * Dargestellt wird Scene mit der Klasse View.
 * TODO: Speichern und laden von Zeichnungen.
 * TODO: Layer-Implementierung
 */

class Scene : public QGraphicsScene
{
public:
    Scene(QObject *parent);
	GridItem* grid() const;
	QPointF snapToGrid(QPointF p) const;

    //void drawBackground ( QPainter * painter, const QRectF & rect );

private:
	GridItem* myGrid;
//    Circle* testCircle;
};

#endif // SCENE_H
