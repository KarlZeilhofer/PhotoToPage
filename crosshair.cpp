#include "crosshair.h"
#include <QGraphicsPathItem>
#include <QPainterPath>
#include <QPen>
#include <Qt>

CrossHair::CrossHair(QGraphicsItem *parent, QGraphicsScene *scene)
        :QGraphicsPathItem(parent, scene)
{
    QPainterPath path;

    setPen(QPen(Qt::blue));

    const int size = 10;

    path.moveTo(-size, 0);
    path.lineTo(size,0);
    path.moveTo(0,-size);
    path.lineTo(0,size);


    //const QPainterPath &refPath = path;
    setPath(path);

    setZValue(1000);
}
