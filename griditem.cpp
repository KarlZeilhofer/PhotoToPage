#include "griditem.h"
#include "view.h"
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QObject>
#include <QGraphicsWidget>
#include <QMessageBox>

GridItem::GridItem(QGraphicsItem * parent = 0) :
	QGraphicsItem(parent) {
	grid = 1; // standard grid of 1mm
	rect = QRectF(0, 0, 297, 210);
	setCacheMode(QGraphicsItem::DeviceCoordinateCache, QSize(rect.width(),
			rect.height()));
	// diese Zeile ermöglicht das schnelle Zeichnen des Grids!!
	gridLines = false;
}

void GridItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
		QWidget *widget) {
	painter->setPen(Qt::darkGray);

	//return immediately if grid is too small on the screen
	if (scene()->views()[0]->matrix().m11() * spacing() < MIN_GRID_SPACING)
		return;

	if (gridLines) {
		for (qreal y = rect.y(); y <= rect.y() + rect.height(); y += grid) {
			painter->drawLine(QPointF(rect.x(), y), QPointF(rect.x()
					+ rect.width(), y));
		}
		for (qreal x = rect.x(); x < rect.x() + rect.width(); x += grid) {
			painter->drawLine(QPointF(x, rect.y()), QPointF(x, rect.y()
					+ rect.height()));
		}
	} else {
		// Gitterpunkte:
		for (qreal y = rect.y(); y <= rect.y() + rect.height(); y += grid) {
			for (qreal x = rect.x(); x < rect.x() + rect.width(); x += grid) {
				painter->drawPoint(qRound(x), qRound(y));
			}
		}
	}
}

/*
 * Input: spacing between grid dots; unit is mm
 */

void GridItem::setSpacing(qreal spacing) {
	grid = spacing;
}

QRectF GridItem::boundingRect() const {
	return rect;
}

//void GridItem::setViewScale(qreal scale) {
//	viewScale = scale;
//}
