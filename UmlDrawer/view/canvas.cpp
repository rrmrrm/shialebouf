
#include "canvas.h"
#include "common.hpp"
#include "shape_drawing/drawing.h"
#include "model/CanvasElements/canvaselement.h"
#include "view/canvaselementwidgetbase.h"
//#include "view/umlcomponentwidget.h"
#include "view/umlclasswidget.h"
#include "canvaselementwidgetcreator.hpp"
#include "model/plug.h"
//#include "model/canvasevent.h"

#include <QWidget>
#include <QColor>
#include <QBrush>
#include <QRect>
#include <QPaintEvent>
#include <QObjectList>
#include <qdebug.h>

#include <iostream>
Canvas::Canvas(Model* m_, QWidget *parent):
	QWidget(parent),
	m(m_)
		{
	//_layout = new QGridLayout();
	(void)parent;
	//isDrawingSet = false;
  
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  
  //setFixedWidth(wid);
	//setFixedHeight(wid);
	//setLayout(_layout);
	setMouseTracking(true);
	
}
Canvas::~Canvas(){
}

CanvasElementWidget* Canvas::viewFor(std::weak_ptr<CanvasElement> ceb){
	if(!ceb.lock())
		return nullptr;
	
	for(auto it: findChildren<CanvasElementWidget*>()){
		if((it->getCE().lock()) && it->getCE().lock() == ceb.lock()){
			return it;
		}
	}
	return nullptr;
}
CanvasElementWidget* Canvas::viewFor(CanvasElement* ceb){
	if(!ceb)
		return nullptr;
	
	for(auto it: findChildren<CanvasElementWidget*>()){
		if((it->getCE().lock()) && it->getCE().lock().get() == ceb){
			return it;
		}
	}
	return nullptr;
}
ConnWidget* Canvas::viewFor(std::weak_ptr<Connector> conn){
	if(!conn.lock())
		return nullptr;
	
	for(auto it: findChildren<ConnWidget*>()){
		if((it->getModel().lock()) && it->getModel().lock() == conn.lock()){
			return it;
		}
	}
	return nullptr;
}

void Canvas::paintEvent(QPaintEvent* ev){
	(void)ev;

	QPainter p(this);
  if(highlight != QRect()){
    p.fillRect(highlight, QBrush(Qt::blue, Qt::SolidPattern));
  }
	QColor defCol(255,255,255);
	QBrush defBrush(defCol,Qt::SolidPattern);
	//p.fillRect(rectToRepaint, defBrush);
	p.setPen(Qt::black);
	if(		m->getLastDrawing()->isBoundsSetup() 
			&& rectToRepaint.intersects(m->getLastDrawing()->getBoundingRect())
			//&&m->getLastDrawing()->isVisible()
			)	{
		
		for(auto gesture : m->getLastDrawing()->getGestures()){
      QPoint lastLineEnd; bool firstLine = true;
			for(auto point : gesture.getPoints()){
				int x = point.first;
				int y = point.second;
				// QPointF ellpseCorner0(x - radius, y - radius);
				// QPointF ellpseCorner1(x + radius, y + radius);
				// QRectF rect(ellpseCorner0, ellpseCorner1);
				// p.drawEllipse(rect);
        
        // vonalat rajzolása:
        QPoint lineEndNew(x,y);
        if(firstLine){
          firstLine = false;
        }
        else{
          p.drawLine(lastLineEnd, lineEndNew);
        }
        lastLineEnd = lineEndNew;
			}
		}
	}
  
  // draw border of canvas	
  qreal lineWidth = 10.0;
  QColor borderColor(0,0,0, 100);
  p.setPen(QPen(QBrush(borderColor, Qt::SolidPattern), lineWidth));
  int w = geometry().width();
  int h = geometry().height();
  p.drawRect(0, 0, w-1, h-1);
  
	//srand(3);
	//auto drawings = m->getDrawings();
	//if(drawings.size() > 0){
	//	auto draw = drawings[drawings.size()-1]->getPixelMatrix();
	//	double scale = 10.0;
	//	///a kép megjelenítésének helyét letisztítom
	//	/// (egy teli téglalpot rajzolok oda):
	//	if(draw.size() > 0){
	//		QRectF clearRect( QPoint(0,0),QPoint(scale*(int)(draw.size()), scale*(int)(draw.at(0).size())) );
	//		p.fillRect(clearRect, QBrush(QColor(0,0,0), Qt::SolidPattern));
	//	}
	//	
	//}
}

void Canvas::Model_CanvasElementAdded(std::weak_ptr<CanvasElement> ce){
	auto obsCE = ce.lock();
	if(!obsCE)
		return;
  std::lock_guard<std::mutex> g(cewsMutex);
	uint newEntityInd = CEWs.size();
	CEWs.push_back(createDerived(ce, this));
	auto entityWidget = CEWs[newEntityInd];
	
	connect(entityWidget, SIGNAL(mousePressEventAcc_C(QMouseEvent*)), this, SLOT(EntityWidgetmousePressEvent(QMouseEvent*)));
	connect(entityWidget, SIGNAL(mouseMoveEventAcc_C(QMouseEvent*)), this, SLOT(EntityWidgetmouseMoveEvent(QMouseEvent*)));
	connect(entityWidget, SIGNAL(mouseReleaseEventAcc_C(QMouseEvent*)), this, SLOT(EntityWidgetmouseReleaseEvent(QMouseEvent*)));
	connect(
		entityWidget, 
		SIGNAL(setGeometry_CEB(QRect)), 
		obsCE.get(),
		SLOT(CEW_setGeometry(QRect))
	);
	connect(obsCE.get(), SIGNAL(geometryChanged_CEW()), entityWidget, SLOT(CE_geometryChanged()));
	//connect(obsCE.get(), SIGNAL(destroyView_W()), entityWidget, SLOT(CE_destroyThis()));
	connect(obsCE.get(), SIGNAL(should_repaint()), entityWidget, SLOT(myRepaint()));
	connect(obsCE.get(), SIGNAL(resizeEdgeSig(int, int/*, Direction*/)), entityWidget, SLOT(CE_resize(int, int/*, Direction*/)));
	connect(obsCE.get(), SIGNAL(drag_CEW(int,int)), entityWidget, SLOT(CE_Drag(int,int)));
	connect(
		obsCE.get(), 
		SIGNAL(ConnectedTo_C(
					std::weak_ptr<CanvasElement>,
					std::weak_ptr<Plug>,
					std::weak_ptr<CanvasElement>, 
					std::weak_ptr<Connector>)
			),
		this, 
		SLOT(CE_ConnectedTo(
				 std::weak_ptr<CanvasElement>,
				 std::weak_ptr<Plug>,
				 std::weak_ptr<CanvasElement>, 
				 std::weak_ptr<Connector>)
			 )
	);

	
	entityWidget->myRepaint(); // entityWidget->smartUpdate();
	entityWidget->show();
}
void Canvas::M_destroyCEW(IdType aModelId){
  std::lock_guard<std::mutex> g(cewsMutex);
	auto found = std::find_if(
		CEWs.begin(),
		CEWs.end(),
		[&aModelId](CanvasElementWidget* aCEW){
			return aCEW != nullptr 
				&& aCEW->getCE().lock()
				&& aCEW->getCE().lock()->getId() == aModelId;
		}
	);
	if(found == CEWs.end()){
		qDebug("warning: Canvas::M_destroyCEW: cew not found with given id");
	}
	else{
		//CEWs.erase(found)
    
    // qt automatically deletes the children of deleted widgets.
    // so i need to find children of 'found' to remove pointers to the deleted children of 'found' from CEWs
    QList<CanvasElementWidget*> children = (*found)->findChildren<CanvasElementWidget*>();
		delete (*found);
    //(*found)->deleteLater();
		(*found) = nullptr;
    CEWs.erase(found);
    // find deleted children, erase them from CEWs.
    for(auto c : children){
      auto found = std::find(
        CEWs.begin(),
        CEWs.end(),
        c
      );
      if(found != CEWs.end()){
        CEWs.erase(found);
      }
    }
	}
}
void Canvas::CE_ConnectedTo(
		std::weak_ptr<CanvasElement> ceS,
		std::weak_ptr<Plug> pl,
		std::weak_ptr<CanvasElement> ceT, 
		std::weak_ptr<Connector> conn)
{
  std::lock_guard<std::mutex> g(cewsMutex);
	CanvasElementWidget* cewS = viewFor(ceS);
	CanvasElementWidget* cewT = viewFor(ceT);
	ConnWidget* cw = viewFor(conn);
	cewS->C_connectedTo(pl, cewT, cw);
}
void Canvas::removeCanvasElementWidgets(){
  std::lock_guard<std::mutex> g(cewsMutex);
  // Tshe widgets are part of a tree hierarchy where if a parent is deleted, all of its children are deleted automatically
  //  so instead of deleting elements of CEWs(which would casue double deleting grandchildren of Canvas), 
  //  i just delete the immediate children of the Canvas, and clear CEWs.
  for(QObject* child : children()){
    delete child;
  }
	//for(auto cew : CEWs){
	//	//cew->deleteLater();
  //  delete cew;
	//	cew = nullptr;
	//}
	CEWs.clear();
}
void Canvas::Update(QRect updatedRect){
	++permitRepaintNum;
	rectToRepaint = updatedRect;
  // this->repaint(updatedRect);
	this->repaint(); 
}
void Canvas::Update(){
	++permitRepaintNum;
	rectToRepaint = geometry();
	this->repaint();
	for(auto& c : findChildren<CanvasElementWidget*>()){
		c->myRepaint(); // c->smartUpdate();
	}
}
bool Canvas::saveAsImage(QString file){
	return grab().save(file);
}
std::weak_ptr<CanvasElement> Canvas::srcAsCE(){
	CanvasElementWidget* asCEW = dynamic_cast<CanvasElementWidget*>(sender());
	if(asCEW){
		if(!asCEW->getCE().lock()){
			qDebug() << "WARNING: Canvas::srcAsCE(): CEW is notnull, but its ce is null! this function will wrongly assume, that the sender was the Canvas!";
		}
		return asCEW->getCE();
	}
	else{
		return std::weak_ptr<CanvasElement>();	
	}
}
/// sender egy CE vagy a Canvas lehet.
/// Visszaadjuk event Canvas-beli koordinátáit:
QPoint Canvas::pToCanv(QMouseEvent* event){
	auto ce = srcAsCE();
	QPoint p = event->localPos().toPoint();
	if(auto ceO = ce.lock()){
		return p + ceO->getGeometry().topLeft();
	}
	return p;
}
ConnWidget* Canvas::getConnWidgetAt(QPoint p){
  for(auto cew : CEWs){
    if(cew){
      if(ConnWidget* cw = cew->getConnWidgetAt(p)){
        return cw;
      }
    }
  }
  return nullptr;
}
void Canvas::mouseEventHelper(QMouseEvent *e){
	std::weak_ptr<CanvasElement> ce;
	/// cPos lesz az esemény Canvas beli koordinátái:
	QPoint cPos = pToCanv(e);
  // highlighting connector under cursor:
  {
  std::lock_guard<std::mutex> g(cewsMutex);
  if(ConnWidget* cw = getConnWidgetAt(cPos)){
    QPoint highlightP0;
    // cw's top-left corner is at QPoint(0,0) in its own coordinate system
    // we convert the coordinate of it's top-left corner to Canvas's coordinate system
    highlightP0 = cw->mapTo(this, QPoint(0,0));
    // we highlight the cw
    highlight = QRect(highlightP0, cw->geometry().size());
    // reaptin canvas to show the highlighting rectangle
    Update();
  }
  else{
    if(highlight  != QRect()){
      highlight = QRect();
      //only repaint Canvas, if highlight changed
      Update();
    }
  }
  }
	m->canvas_mouseEvent(e->clone(), srcAsCE(), cPos);
}
void Canvas::mousePresEventHelper(QMouseEvent *e){
	std::weak_ptr<CanvasElement> ce;
	/// cPos lesz az esemény Canvas beli koordinátái:
	QPoint cPos = pToCanv(e);
	m->canvas_mouseEvent(e->clone(), srcAsCE(), cPos);
}
void Canvas::mousePressEvent(QMouseEvent *e){
	mousePresEventHelper(e);
}
void Canvas::mouseMoveEvent(QMouseEvent *e){
  // steal focus from any TextBoxWidget, so keypresses like ENTER arrive to the Canvas and propagate to the Model
  this->setFocus();
	mouseEventHelper(e);
}
void Canvas::mouseReleaseEvent(QMouseEvent *e){
	mouseEventHelper(e);
}


void Canvas::EntityWidgetmousePressEvent(QMouseEvent *e){
	mousePresEventHelper(e);
}
void Canvas::EntityWidgetmouseMoveEvent(QMouseEvent *e){
	mouseEventHelper(e);
}
void Canvas::EntityWidgetmouseReleaseEvent(QMouseEvent *e){
	mouseEventHelper(e);
}
void Canvas::EntityWidgetmouseDoubleClickEvent(QMouseEvent *e){
	mousePresEventHelper(e);
}








