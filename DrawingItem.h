/*
 * DrawingItem.h
 *
 *  Created on: 21.12.2009
 *      Author: Karl Zeilhofer
 */

#ifndef DRAWINGITEM_H_
#define DRAWINGITEM_H_

#include <QGraphicsItem>
#include <QList>

/*
 * Abstrakte Klasse für zu zeichnende Grafik-Objekte.
 * Wird hauptsächlich dafür benötigt, um ein einheitliches Interface für das Zeichnen
 * zur Verfügung zu Stellen. Grafik-Objekte benötigen einen oder mehrere Punkte
 * zur Definition.
 * TODO: Deklaration von Eigenschaften wie: ID, Strichstärke, Layer, Farbe
 * TODO: Standard Selekt, Drag und resize Verhalten implementeieren bzw. rechachieren.
 *
 * Beim Erstellen eines Grafik-Objekts wird die Methode addPoint() genügend oft aufgerufen,
 * damit ein Grafik-Obkjekt konstruiert werden kann.
 *
 * Eine Abstrakte Klasse zeichnet aus, dass sie eine virtual-Methode deklariert mit der Kennzeichnung =0
 */
class DrawingItem : public QGraphicsItem{

public:
	DrawingItem(QGraphicsItem *parent, QGraphicsScene *scene)
		: QGraphicsItem(parent, scene){

	}
	virtual ~DrawingItem(){};

	// folgende Methoden müssen noch in Unterklassen überschrieben werden!!!
	// rein abstrakte Methoden werden durch =0 deklariert!
	virtual int addPoint(QPointF p)=0; // Punkt hinzufügen,
		//gibt die Anzahl an noch benötigten Punkten zurück
	virtual int pointsLeft()=0; //gibt die Anzahl an noch benötigten Punkten zurück
	virtual void updateLastPoint(QPointF p)=0; // Aktualisiert den zuletzt hinzugefügten Punkt

public:
    QString name; // a name for the drawn item. can be used as an ID

protected:
	QList<QPointF> points; // Eingabe-Punkte beim Erstellen
	int neededPoints; // benötigte Anzahl an Punkten zur eindeutigen Konstruktion
};

#endif /* DRAWINGITEM_H_ */
