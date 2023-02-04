#ifndef CANVAS_H
#define CANVAS_H

#include "model/model.h"
#include "shape_drawing/drawing.h"
#include "shape_drawing/gesture.h"
#include "canvaselementwidgetbase.h"
#include "common.hpp"


#include <QPainter>
#include <QWidget>
#include <QLineEdit>
#include <QGridLayout>

#include <iostream>
#include <vector>
#include <mutex>
class Canvas: public QWidget {
	Q_OBJECT
	
	Model* m;
	std::vector<CanvasElementWidget*> CEWs;
  std::mutex cewsMutex;
	QRect rectToRepaint;
	int permitRepaintNum = 0;
  QRect highlight;
public:
	Canvas(Model* m, QWidget *parent = nullptr);
	~Canvas();
public:
	CanvasElementWidget* viewFor(std::weak_ptr<CanvasElement> ceb);
	CanvasElementWidget* viewFor(CanvasElement* ceb);
	ConnWidget* viewFor(std::weak_ptr<Connector> conn);
public slots:
	void paintEvent(QPaintEvent* e) override;
	void Model_CanvasElementAdded(std::weak_ptr<CanvasElement> ce);
	void M_destroyCEW(IdType);
	///
	/// \brief CE_ConnectedTo
	/// Az egyszerűbb CEW-ek csatlakoztatására.
	/// Mikor a modellben megtörténik a Plug-Connector csatlakoztatás, ezzel érdemes a CE-k nézeteit is csatlakoztatni
	/// a LineWidget UMLClass-hoz csatlakoztatásához nem használom ezt az eljárást.
	/// 
	/// Megkeresi ceS, ceT és conn nézetét,
	/// és csatlakoztatja ceS és ceT nézetét. ehhez pl-t és conn nézetét felhasználja.
	/// \param ceS : ennek a CEW-nek a nézetét kell conn nézetére csatlakoztatni
	/// \param pl: ceS csatlakoztatott Plug-ja(itt nem kell csatlakoztatni sehova, ez a modell rétegben történik meg)
	/// \param ceT 
	/// \param conn : ceT Connector -ja, amire a modellben felcsatlakoztattuk pl-t.
	///
	void CE_ConnectedTo(std::weak_ptr<CanvasElement> ceS,
					 std::weak_ptr<Plug> pl,
					 std::weak_ptr<CanvasElement> ceT, 
					 std::weak_ptr<Connector> conn);
	void removeCanvasElementWidgets();
	void Update(QRect);
	void Update();
	//Direction cewEdgeAt(QPoint cPoint, CanvasElementWidget** cewOut);
	bool saveAsImage(QString file);
	std::weak_ptr<CanvasElement> srcAsCE();
	/// sender egy CE vagy a Canvas lehet.
	/// Visszaadjuk event Canvas-beli koordinátáit:
	QPoint pToCanv(QMouseEvent* event);
  ConnWidget* getConnWidgetAt(QPoint); // to highlight connWidget under cursor
  
	void mouseEventHelper(QMouseEvent*);
	void mousePresEventHelper(QMouseEvent*);
	void mouseMoveEvent(QMouseEvent*) override;
	void mouseReleaseEvent(QMouseEvent*) override;
	void mousePressEvent(QMouseEvent*) override ;
	
	//a CEW -ek által továbbított MouseEventekre reagálnak ezek a SLOT-ok:
	void EntityWidgetmousePressEvent(QMouseEvent *event) ;
	void EntityWidgetmouseMoveEvent(QMouseEvent *event);
	void EntityWidgetmouseReleaseEvent(QMouseEvent *event);
	void EntityWidgetmouseDoubleClickEvent(QMouseEvent *event);
};

#endif // CANVAS_H
