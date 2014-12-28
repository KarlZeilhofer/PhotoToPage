
/*
  This Class is used to display the grid dots in the background of a scene.

*/

#ifndef GRIDITEM_H
#define GRIDITEM_H

#include <QGraphicsItem>

// TODO: SIGNAL-SLOT für viewScale implementieren!!!!
class GridItem : public QGraphicsItem
{
public:
    GridItem(QGraphicsItem * parent);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    qreal spacing() const {return grid;}
    void setSpacing(qreal spacing);
    void setRect(QRect rect);
    QRectF boundingRect() const;

private:
    qreal grid; // grid spacing between grid dots
    QRectF rect; // bounds of grid area
    bool gridLines; // if true: paint grid LINES instead of grid DOTS

    static const int MIN_GRID_SPACING = 5; // unit: Pixels
    qreal viewScale; // scale factor of the View, which displays the grids scene
};

#endif // GRIDITEM_H
