#include "umlclasswidget.h"
#include "shape_drawing/drawing.h"
#include "shape_drawing/gesture.h"
#include "model/CanvasElements/canvaselement.h"
#include "ui_umlClass.h"

#include <QPainter>
#include <QColor>
#include <QBrush>
#include <QRect>
#include <QPaintEvent>
#include <QBoxLayout>
#include <QFrame>
#include <qdebug.h>

///TODO: megoldani hogz a umlclassWidgetbeli textbox átméretezésekor átméeteződjön a widget is,
//// umlwidgetet .ui filebol a connwidget2 tipusu connectorokat felhasynalni
umlClassWidget::umlClassWidget(std::shared_ptr<UMLClassComponent> uc_, QWidget* parent)
	:	
		CanvasElementWidget(uc_, parent),
		ucc(std::dynamic_pointer_cast<UMLClassComponent>(uc_)),
		ui(new Ui::umlClass)
		{
	ui->setupUi(this);
	auto uccO = ucc.lock();
	assert(uccO);
	auto ceObs = ce.lock();
	assert(ceObs);
	ui->connLeft->setModel(uccO->getConnLeft());
	ui->connRight->setModel(uccO->getConnRight());
	ui->connTop->setModel(uccO->getConnTop());
	ui->connBot->setModel(uccO->getConnBottom());
	
	ui->connTitle->setModel(uccO->getConnTitle());
	ui->connFields->setModel(uccO->getConnFields());
	ui->connMethods->setModel(uccO->getConnMethods());
	setGeometryInC(ceObs->geometry);
	
	ui->connTitle->setFixedHeight(titleHeight);
	//vbl = new QVBoxLayout(this);
	//setLayout(vbl);
	auto mObs = ucc.lock();
	assert(mObs);
  setMouseTracking(true);
	
	//auto cwt = new ConnWidget(mObs->getConnTitle());
	//auto cwf = new ConnWidget(mObs->getConnFields());
	//auto cwm = new ConnWidget(mObs->getConnMethods());
	//
	//auto cwLeft = new ConnWidget(mObs->getConnLeft());
	//auto cwRight = new ConnWidget(mObs->getConnRight());
	//auto cwTop = new ConnWidget(mObs->getConnTop());
	//auto cwBottom = new ConnWidget(mObs->getConnBottom());
	
	//vbl->addWidget(cwt, 1);
	//vbl->addWidget(makeHSeparator());
	//vbl->addWidget(cwf, 1);
	//vbl->addWidget(makeHSeparator());
	//vbl->addWidget(cwm, 1);
	//
	//cwf->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
	//cwm->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
	//setLayout(vbl);
	//qDebug() << "cwt->geometry(); \n";
	//qDebug() << cwt->geometry() << "\n";
	//qDebug() << cwf->geometry() << "\n";
	//qDebug() << cwm->geometry() << "\n";
}
umlClassWidget::~umlClassWidget(){
	delete ui;
}
QFrame* umlClassWidget::makeHSeparator(QWidget* parent){
	QFrame *line;
	line = new QFrame(parent);
	line->setFrameShape(QFrame::HLine);
	line->setFrameShadow(QFrame::Sunken);
	return line;
}

void umlClassWidget::derivedPaintEvent(QPaintEvent* e){
	auto ceObs = ce.lock();
	if(!ceObs)
		return;
	QPainter p(this);
	int w = width();
	int h = height();
  int lineWidth = 3;
  // az uml osztály 4 oldalsó élének kirajzolása:
  p.setPen(QPen(QBrush(Qt::black, Qt::BrushStyle::SolidPattern), lineWidth));
  p.drawRect(0,0, w-1,h-1);
}
void umlClassWidget::CE_geometryChanged(){
	
}
