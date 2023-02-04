#include "canvaselementwidgetbase.h"
//#include "linewidget.h"
#include "view/canvas.h"
#include "view/connwidget.h"
#include <model/plug.h>

#include <QMouseEvent>
#include <QEventLoop>
#include <qdebug.h>

CanvasElementWidget::CanvasElementWidget(
			std::weak_ptr<CanvasElement> ce_,
			QWidget* parent
		)
	:	
		QWidget(parent),
		ce(ce_)
{
  setMouseTracking(true);
}
CanvasElementWidget::~CanvasElementWidget(){
	
}

QRect CanvasElementWidget::toWCoords(QRect rInCanvas) const{
	return rInCanvas.translated(-x(),-y());
}
QRect CanvasElementWidget::getGeomInC(){
	QPoint p0 = mapTo(accessCanvas(), QPoint(0, 0));
	return QRect(p0, rect().size());
}
std::weak_ptr<CanvasElement> CanvasElementWidget::getCE() const{
	return ce;
}
void CanvasElementWidget::setGeometryInC(QRect rInC){
	/// a setGeometry utasítással fogom beállítani a CEW geometriáját.
	/// az új geometria a Canvas-hoz relatív rInC. 

  
	// Nekem CEW szülőjéhez relatíven kell.
	// CEW szülője lehet egy másik CEW, vagy maga a Canvas
	
	// rInC téglalap bal-felső pontja a CEW-hez képest:
	QPoint p0InCEW = mapFrom(accessCanvas(), rInC.topLeft());
	// rInC balfelső pontja a CEW szülőjéhez képest(a szülője lehet a Canvas, vagy egy másik CEW is):
	QPoint p0InP = mapToParent(p0InCEW);
	QRect rInP = QRect(p0InP, rInC.size());
	setGeometry(rInP);
}
Canvas* CanvasElementWidget::accessCanvas(){
	QObject* p = parent();
	while(p != nullptr){
		if(auto casted = dynamic_cast<Canvas*>(p))
			return casted;
		else
			p = p->parent();
	}
	assert(false);
	return nullptr;
}
void CanvasElementWidget::paintSelectionoutline(){
	QPainter p(this);
	auto co = ce.lock();
	if(!co)
		return;
	//auto savedPen = p->pen();
	
	int thickness = 3;
	QColor selectHightlightC = QColor(0, 248, 0, 200);
	p.setPen(QPen(selectHightlightC, 3));
	if(co->isSelected()){
		p.drawRect(QRect(0,0,width()-thickness,height()-thickness));
	}
}
void CanvasElementWidget::ConnW_wantsToResize(ConnWidget* cw, QSize newSize){
	assert(cw);
	/// az alapműködés az hogy egyszerűen végrehajtjuk a kért átméretezést,
	/// cw ennek hatására beállítja a modelljének(Connector) is az új méretet...
	cw->resize(newSize);
	/// ...és frissítjük a nézetet(CEW):
	CE_geometryChanged();
}
void CanvasElementWidget::C_connectedTo(std::weak_ptr<Plug>, CanvasElementWidget*, ConnWidget* cw){
	/// ha nem a Canvas volt a parentje, akkor hiba történt
	if(parent() != accessCanvas()){
		qDebug("WARNING: CanvasElementWidget::C_connectedTo(std::weak_ptr<Plug>, CanvasElementWidget*, ConnWidget* cw): parent was not the canvas");
		setParent(nullptr);
	}

	setParent(cw);
	assert(cw->layout());
	cw->layout()->addWidget(this);
	setVisible(true);// ez meghívja CEW::moveEvent-et, így modell és Connectorai geometrije frissül itt
	/// felülre hozzuk a widget-et-t:
	raise();
	repaint();
	repaint();// smartUpdate();
}

ConnWidget* CanvasElementWidget::getConnWidgetAt(QPoint canvasPoint){
  if(QWidget* c = childAt(mapFrom(accessCanvas(), canvasPoint))){
    if(auto cw = qobject_cast<ConnWidget*>(c)){
      return cw;
    }
  }
  return nullptr;
}
void CanvasElementWidget::resizeEvent(QResizeEvent* ){
	for(auto connW : findChildren<ConnWidget*>()){
		connW->resizeEvent(nullptr);
	}
  // ha a qt keretrendszer méretezte át a CEW-et,
  // akkor a modell még nem ismeri az új geometry-t ezért itt frissítjük
  emit setGeometry_CEB(getGeomInC());
}
void CanvasElementWidget::moveEvent(QMoveEvent *){
	for(auto connW : findChildren<ConnWidget*>()){
		connW->resizeEvent(nullptr);
	}
  // ha a qt keretrendszer méretezte át a CEW-et,
  // akkor a modell még nem ismeri az új geometry-t ezért itt frissítjük
  emit setGeometry_CEB(getGeomInC());

}

void CanvasElementWidget::myRepaint(){
	repaint();
}
void CanvasElementWidget::CE_geometryChanged(){
	if(auto ceObs = ce.lock()){
		setGeometryInC(ceObs->geometry);
		repaint();// smartUpdate();
	}
}

void CanvasElementWidget::paintEvent(QPaintEvent* e){
		derivedPaintEvent(e);
		paintSelectionoutline();
}
void CanvasElementWidget::derivedPaintEvent(QPaintEvent* e){
	(void)e;
}

void CanvasElementWidget::mousePressEvent(QMouseEvent *event){
		event->accept();
		emit mousePressEventAcc_C(event);

}
void CanvasElementWidget::mouseMoveEvent(QMouseEvent *event){
	event->accept();
	emit mouseMoveEventAcc_C(event);
}
void CanvasElementWidget::mouseReleaseEvent(QMouseEvent *event){
	emit mouseReleaseEventAcc_C(event);
}
void CanvasElementWidget::CE_Drag(int cX, int cY){
	//auto cew = accessCanvas();
	auto cso = size();
	/// ha a cew egy másik cew connW-jében volt, akkor most kivesszük,
	/// és közvetlenül a Canvasba tesszük vissza:
	if(parent() != accessCanvas()){
		auto cebp = dynamic_cast<ConnWidget*>(parent());
		// ///itt szólok a connwidgetnek vagy parentjének, hogy kivettem a widgete belőle
		// cebp->resetSize();
		assert(cebp);
		auto pLay = cebp->layout();
		assert(pLay);
		pLay->removeWidget(this);
		
		//assert(_layout);
		setParent(accessCanvas());
		setVisible(true);
		setGeometry(cX-1, cY-1, cso.width(), cso.height());
	}
	else{
		move(cX, cY);
		emit setGeometry_CEB(QRect(cX, cY, cso.width(), cso.height()));
	}
	/// előre hozom a cew-t:
	raise();
	repaint();
	repaint();// smartUpdate();
}
void CanvasElementWidget::CE_resize(int cX, int cY){
	QPoint local = mapFrom(accessCanvas(), QPoint(cX, cY));
	
	resize(local.x(), local.y());
	repaint();
}
