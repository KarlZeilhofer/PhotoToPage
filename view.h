#ifndef VIEW_H
#define VIEW_H

#include <QGraphicsView>
#include <QGraphicsItem>
#include "DrawingItem.h"
#include "rotatedrect.h"

#include <QObject> // wird für selbst gemachte SLOTS benötigt!

class QGraphicsScene;
class QGraphicsItem;
class CrossHair;
class Scene;



class View : public QGraphicsView
{
	Q_OBJECT // wird für selbst gemachte SLOTS benötigt!

public:
    View(QWidget *parent);
	Scene* getScene() const;
    void setVisibleRectangle(QRectF sceneRect);
    void stopCurrentTool();

private:
    void wheelEvent(QWheelEvent *event); // überladen von QGraphicsItem::wheelEvent()
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
	//void drawForeground(QPainter *painter, const QRectF &rect);

public slots:
//    void newCircle();
//    void newRectangle();
    void newRotatedRect(RotatedRect::Type type);

signals:
    void scaleChanged(qreal scale);
    void finishedRotatedRect(RotatedRect *cutout);

private:
	Scene* scene;
    CrossHair *crossHair;
    DrawingItem* newItem;
    static const qreal zoomFactor=1.2;
    int drawingState;
        // 0: no tool selected
        // 1: tool selected, add point on mouse-move, waiting for first click
        // 2: add additional points, update last point on mouse-move, waiting for last point
        // 3: last point was added, update last point on mouse-move, emit finishedXxx signal
    enum Tool{Tool_None, Tool_Page, Tool_Thumb} currentTool;

    /* TODO: Ideensammlung für Grafikobjekte:
     * 1-Punkt: Point
     * 2-Punkt: Circle (center, radius),
     * 3-Punkt: Ellipse (center, rx,ry),
     * 			Ellipse (focus1, focus2, radius),
     * n-Punkt: Line, Spline
     */

};

#endif // VIEW_H
