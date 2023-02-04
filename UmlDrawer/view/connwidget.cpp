#include "connwidget.h"
#include "view/canvas.h"

#include <QPainter>
#include <qdebug.h>
ConnWidget::ConnWidget(QWidget* parent) 
	: QWidget(parent)
{
  setMouseTracking(true);
	l =  new QVBoxLayout(this);
	setMinimumHeight(minHeight);
	l->setContentsMargins(0,0,0,0);
	updateConnGeom();
}
ConnWidget::ConnWidget(std::weak_ptr<Connector> modelConn_, QWidget* parent) 
	: QWidget(parent), 
	  modelConn(modelConn_)
{
	assert(modelConn.lock());
  setMouseTracking(true);
	connect(
		this,
		SIGNAL(setGeometry_Conn(QRect)), 
		modelConn.lock().get(), 
		SLOT(CW_setGeometry(QRect))
	);
  
	
	l =  new QVBoxLayout(this);
	setMinimumHeight(minHeight);
  l->setContentsMargins(0,0,0,0);
	updateConnGeom();
}
ConnWidget::ConnWidget(std::weak_ptr<Connector>  modelConn_, QRect, QWidget* parent) 
	: QWidget(parent), 
	  modelConn(modelConn_)
{
  setMouseTracking(true);
	connect(
		this,
		SIGNAL(setGeometry_Conn(QRect)), 
		modelConn.lock().get(), 
		SLOT(CW_setGeometry(QRect))
	);
	
	l =  new QVBoxLayout(this);
	setMinimumHeight(minHeight);
  l->setContentsMargins(0,0,0,0);
	updateConnGeom();
}
ConnWidget::~ConnWidget(){
	
}

Canvas* ConnWidget::accessCanvas(){
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
void ConnWidget::setGeometry(QRect r){
	QWidget::setGeometry(r);
}
void ConnWidget::setModel(std::weak_ptr<Connector> modelConn_){
	assert(modelConn_.lock());
	modelConn = modelConn_;
	disconnect(this, SIGNAL(setGeometry_Conn(QRect)));
	connect(
		this,
		SIGNAL(setGeometry_Conn(QRect)), 
		modelConn.lock().get(), 
		SLOT(CW_setGeometry(QRect))
	);
}
std::weak_ptr<Connector> ConnWidget::getModel(){
	assert(modelConn.lock());
	return modelConn;
}
//bool ConnWidget::isUnderMouse() const {
//	qDebug() << "isUnderMouse: QCursor::pos(): " << QCursor::pos();
//	qDebug() << "isUnderMouse: apFromGlobal(QCursor::pos()): " << mapFromGlobal(QCursor::pos());
//	qDebug() << "isUnderMouse: rect(): " << rect();
//	
//	qDebug() << "isUnderMouse: rect().contains(mapFromGlobal(QCursor::pos())): " 
//			 << rect().contains(mapFromGlobal(QCursor::pos()));
//    return rect().contains(mapFromGlobal(QCursor::pos()));
//}

// void ConnWidget::resetSize(){
// }

//void ConnWidget::paintEvent(QPaintEvent* e) {
//	(void)e;
//  // the ConnWidget is only visible if mouse is above:
//  if(!isMouseAbove)
//    return;
//	QPainter p = QPainter(this);
//	QBrush b = QBrush(QColor(0,250,0, 70), Qt::SolidPattern);
//	p.fillRect(QRect(0,0, width(), height()), b);
//}
void ConnWidget::updateConnGeom(){
	QPoint p0 = mapTo(accessCanvas(), QPoint(0, 0));
	QRect r(p0, geometry().size());
	emit setGeometry_Conn(r);
}
//void ConnWidget::setIsMouseAbove(bool newVal){
//  isMouseAbove = newVal;
//}
void ConnWidget::resizeEvent(QResizeEvent* ){
	//qDebug() << "ConnWidget::resizeEvent called";
	updateConnGeom();
}
void ConnWidget::moveEvent(QMoveEvent *){
	//qDebug() << "ConnWidget::moveEvent called";
	updateConnGeom();
}

void ConnWidget::mouseMoveEvent(QMouseEvent* e) {
  QWidget::mouseMoveEvent(e);
  //e->ignore();
}
void ConnWidget::mouseReleaseEvent(QMouseEvent* e) {
  QWidget::mouseMoveEvent(e);
  //e->ignore();
}
void ConnWidget::mousePressEvent(QMouseEvent* e) {
  QWidget::mouseMoveEvent(e);
  //e->ignore();
}
