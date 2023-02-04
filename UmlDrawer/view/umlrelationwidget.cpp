#include "umlrelationwidget.h"
#include "view/canvaselementwidgetbase.h"
#include "model/CanvasElements/canvaselement.h"

#include <QPainter>
#include <QPainterPath>

UmlRelationWidget::UmlRelationWidget(std::weak_ptr<UMLRelationship> ur_, QWidget* parent)
	:
	CanvasElementWidget(ur_, parent)
		{
	auto urO = ur_.lock();
	assert(urO);
	setGeometryInC(urO->geometry);
  setMouseTracking(true);
}

const std::shared_ptr<UMLRelationship> UmlRelationWidget::getUr() const{
	auto ceObs = ce.lock();
	return std::dynamic_pointer_cast<UMLRelationship>(ceObs);
}

void rotPainter(QPainter& p, Direction dir){
	switch(dir){
	case TOP:break;
	case RIGHT:{
		p.rotate(90);
		break;
	}
	case BOT:{
		p.rotate(180);
		break;
	}
	case LEFT:{
		p.rotate(-90);
		break;
	}
	}
}
void UmlRelationWidget::derivedPaintEvent(QPaintEvent* ev){
	auto ceObs = ce.lock();
	if(!ceObs)
		return;
	QPainter p(this);
	
	int w = ceObs->width();
	int h = ceObs->height();
	int wh = w / 2;
	int hh = h / 2;
	QColor defCol = Qt::white;
	QBrush defBrush(defCol, Qt::SolidPattern);
	//szabványos uml komponens kirajzolása, ha a drawing nem nullptr:
	auto ur = getUr();
	assert(ur != nullptr);
	//auto drawing = ce->dInd < (int)ce->drawings.size() ? ce->drawings[ce->dInd] : nullptr;
	
  // függőségjelölők körvonalához qPen beállítása
	qreal lineWidth = 1.0;
	p.setPen(QPen(QBrush(Qt::black, Qt::SolidPattern), lineWidth));
	switch(ur->getType()){
  case EXTENSIONT:{
    p.setBrush(QBrush(Qt::white));
		//háromszöget nyilat    ^
		//                    /___\  rajzolunk:
    //                      |
		p.translate(QPoint(wh,hh));
		//p.scale(w/7.0, h/7.0);
		rotPainter(p, ur->getDir());
    int hh2 = hh;
    int wh2 = wh;
    if(ur->getDir() == Direction::LEFT || ur->getDir() == Direction::RIGHT){
      // ha merőlegesen fordíytottuk el a paintert, akkor meg kell cseélni wh-t és hh-t,
      // hogy ne az elforgatott határoló téglalap alapján legyen skálázva a nyíl.
      int temp = wh;
      wh2 = hh;
      hh2 = temp;
    }
    QPolygon triangle;
    // háromszög:
    triangle << QPoint(-wh2, 0);
    triangle << QPoint(0, -hh2);
    triangle << QPoint(wh2, 0);
    p.drawPolygon(triangle);
		// leszármzás jelölő szára:
		p.drawLine(0, 0, 0, hh2);
		// // vízszintes:
		// p.drawLine(-wh2, 0, wh2, 0);
		break;
	}
  // commenting out this , because cant recognise with NN properly
  //case DIRECTEDASSOCT:{
	//	//            nyilat    ^
	//	//                    / | \  rajzolunk:
  //  //                      |
	//	p.translate(QPoint(wh,hh));
	//	//p.scale(w/7.0, h/7.0);
	//	rotPainter(p, ur->getDir());
  //  int hh2 = hh;
  //  int wh2 = wh;
  //  if(ur->getDir() == Direction::LEFT || ur->getDir() == Direction::RIGHT){
  //    // ha merőlegesen fordíytottuk el a paintert, akkor meg kell cseélni wh-t és hh-t,
  //    // hogy ne az elforgatott határoló téglalap alapján legyen skálázva a nyíl.
  //    int temp = wh;
  //    wh2 = hh;
  //    hh2 = temp;
  //  }
  //  // nyil feje:
  //  QPoint p1(-wh2, 0);
  //  QPoint p2(0, -hh2);
  //  QPoint p3(wh2, 0);
  //  // a nyíl fejét megvastagítom, hogy jobban látsszon
  //  p.setPen(QPen(QBrush(Qt::black), lineWidth+1));
  //  p.drawLine(p1,p2);
  //  p.drawLine(p2, p3);
  //  
	//	// irányított asszociációjelölő szára:
  //  p.setPen(QPen(QBrush(Qt::black), lineWidth));
	//	p.drawLine(0, -hh2, 0, hh2);
	//	break;
	//}
	case AGGREGATIONT:{
		// rombuszt   /\   rajzolunk
		//            \/
		p.translate(QPoint(wh,hh));
		QPolygon romb;
    romb << QPoint(-wh, 0)
         << QPoint(0, -hh)
         << QPoint(wh, 0)
         << QPoint(0, hh);
    p.drawPolygon(romb);
		break;
	}
	case CIRCLET:{
		p.translate(QPoint(wh,hh));
    p.setBrush(Qt::black);
    p.setPen(Qt::black);
    w = std::min(w, h); // circular it will be
    h = w;
		p.drawEllipse(-wh, -hh, w, h);
		break;
	}
	case COMPOSITIONT:{
		//nyújtott teli  rombuszt   /\   rajzolunk
		//                          \/
    p.setBrush(QBrush(Qt::black));// telítéshez
		p.translate(QPoint(wh,hh));
		QPolygon compPoly;
		compPoly 
				<< QPoint(-wh,0)
				<< QPoint(0,-hh)
				<< QPoint(wh,0)
				<< QPoint(0,hh);
    p.drawPolygon(compPoly);
		break;
	}
	default:{
		assert(false);
		break;
	}
	};
}
