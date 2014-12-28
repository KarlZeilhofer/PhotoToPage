#ifndef CROSSHAIR_H
#define CROSSHAIR_H

#include <QGraphicsPathItem>

class CrossHair : public QGraphicsPathItem
{
public:
    CrossHair(QGraphicsItem *parent, QGraphicsScene *scene);
};

#endif // CROSSHAIR_H
