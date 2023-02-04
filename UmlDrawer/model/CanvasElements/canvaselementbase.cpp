#include "canvaselementbase.h"
#include "model/model.h"
#include "model/CanvasElements/canvaselement.h"
#include "model/plug.h"
#include "model/connector.h"
#include "model/target.h"
#include "persistence/streamoperators.h"

#include <QObject>
CanvasElement::CanvasElement(Model* model_,CanvasElement_Type type_, QRect geometry_) 
	: model(model_)
	, CEtype(type_)
	, geometry(geometry_)
	, lastDragPoint(0,0)
{}
CanvasElement::~CanvasElement(){
}

void CanvasElement::moveTopLeft(int cX, int cY){
	geometry.moveTopLeft(QPoint(cX, cY));
	emit geometryChanged_CEW();
}
MyCompositionT<CanvasElement> CanvasElement::createCanvasElement(Model* model, CanvasElement_Type ce_type, QRect area){
	switch (ce_type) {
	case ERROR :{
    // triin to craete error type CanvasElement doesn't make sense.
		assert(false);
		break;
	}
	case LINET :{
		return std::make_shared<LineComponent>(model, area);
		///a LineComponent csak félkész lesz: a végpontjait még be kell állítani ezen eljárás meghívása után.
		break;
	}
	case TEXTBOXT :{
		return std::make_shared<TextComponent>(model, area);
		break;
	}
	case UMLCLASST :{
		return std::make_shared<UMLClassComponent>(model, area);
		break;
	}
	case AGGREGATIONT :{
		return std::make_shared<UMLAggregationSpecifier>(model, area);
		break;
	}
	case COMPOSITIONT :{
		return std::make_shared<UMLCompositionSpecifier>(model, area);
		break;
	}
  case EXTENSIONT :{
		return std::make_shared<UMLExtensionSpecifier>(model, area);
		break;
	}
  // commenting out this , because cant recognise with NN properly
  //case DIRECTEDASSOCT :{
	//	return std::make_shared<UMLDirectedAssociationSpecifier>(model, area);
	//	break;
	//}
	case CIRCLET :{
		return std::make_shared<UMLCircle>(model, area);
		break;
	}
	default:{
    // ce_type ismeretlen*(nincs hooya CE tipus), ezert nullptrt adunk vissza
    return std::shared_ptr<TextComponent>();// nullptr
		//assert(false);
		break;
	}
	}
  return std::shared_ptr<TextComponent>();// nullptr
	//return std::make_shared<TextComponent>(model, area);//vissza kell valamit adni, hogy a compiler ne izéljen
}
std::ostream& CanvasElement::saveAggregations(std::ostream& os){
	//base::saveAggsChainCallThis(..)
	saveContainer(os, connectors);
	saveContainer(os, plugs);
	return os;
}
std::istream& CanvasElement::loadAggregations(std::istream& is){
	//base::loadAggsFinalizeChainCallThis(..)
	loadContainer(is, connectors);
	loadContainer(is, plugs);
	return is;
}
std::ostream& CanvasElement::save(std::ostream& os){
	const std::string idToken = "Identifiable";
	const std::string cebToken = "CanvasElement";
	
		os << idToken << std::endl;
	Identifiable::save(os);
		os << cebToken << std::endl;
	os << CEtype << std::endl;
	os << isSelected_ << std::endl;
	
	os << geometry << std::endl;
	os << dragStartX << std::endl;
	os << dragStartY << std::endl;
	os << lastDragPoint << std::endl;
	os << isDragging << std::endl;
	return os;
}
std::istream& CanvasElement::load(std::istream& is){
	const std::string idToken = "Identifiable";
	const std::string cebToken = "CanvasElement";
	std::string checkStr = "";
	
		is >> checkStr; assert(checkStr == idToken);
	Identifiable::load(is);
		is >> checkStr; assert(checkStr == cebToken);
	//CEType konstans az objektum létrehozásakor helyesen beálítódik. a helyes beállításáról létrehozásakor gondoskodunk. nem kel betölteni:
	int icet = 0; is >> icet; assert(is);// CEtype = (CanvasElement_Type)icet;
	is >> isSelected_;
	
	is >> geometry;
	is >> dragStartX;
	is >> dragStartY;
	is >> lastDragPoint;
	is >> isDragging;
	return is;
}
void CanvasElement::dragUnselectedOverrideThis(int canvas_x, int canvas_y){
	emit drag_CEW(canvas_x, canvas_y);
}
void CanvasElement::stopDragOverrideThis(QPoint cPos){
	//emit connectToCEAt(weak_from_this(), cPos, getMainPlug());
  model->CE_connectToCEAt(weak_from_this(), cPos, getMainPlug());
}

QRect CanvasElement::getGeometry(){
	return geometry;
}
std::weak_ptr<Connector> CanvasElement::getConnAt(QPoint p){
	for(auto conn : connectors){
		if(auto connO = conn.lock()){
			if(connO->getGeometry().contains(p)){
				return conn;
			}
		}
	}
	return std::weak_ptr<Connector>();
}
std::weak_ptr<Connector> CanvasElement::getConnAt(int cX, int cY){
	return getConnAt(QPoint(cX, cY));
}

std::vector<AggregationT<Connector>> CanvasElement::getConnectors(){
	return connectors;
}
bool CanvasElement::containsPoint(QPoint p) const{
	return geometry.contains(p);
}
/// TODO: minden leszármazó canvasElement-ben feltölteni és managel-ni a connector-s vektort, hogy ez az eljárás általánosan használható legyen:
std::weak_ptr<Connector> CanvasElement::getConnFirstAtCanvasCoords(QPoint coords){
	auto conns = getConnectors();
	for(int i = 0 ; i < (int)conns.size() ; ++i){
		if(auto co = conns[i].lock()){
			if(co->containsPoint(coords)){
				return co->weak_from_this();
			}
		}
	}
	return std::weak_ptr<Connector>();
}
/// TODO: minden leszármazó canvasElement-ben feltölteni és managel-ni a connector-s vektort, hogy ez az eljárás általánosan használható legyen:
std::vector<AggregationT<Connector>> CanvasElement::getConnsAllAtCanvasCoords(QPoint coords){
	std::vector<AggregationT<Connector>> ret;
	for(int i = 0 ; i < (int)connectors.size() ; ++i){
		if(auto co = connectors[i].lock()){
			if(co->containsPoint(coords)){
				ret.push_back(co->weak_from_this());
			}
		}
	}
	return ret;
}
int CanvasElement::x() const {return geometry.x();}
int CanvasElement::y() const {return geometry.y();}
int CanvasElement::width() const {return geometry.width();}
int CanvasElement::height() const {return geometry.height();}
void CanvasElement::dragUnselected(int canvas_x, int canvas_y){
	bool first = !isDragging;
	dragUnselectedOverrideThis(canvas_x, canvas_y);
	if(first){
		isDragging = true;
	}
	//emit drag_CEW(canvas_x, canvas_y);
	
	//emit geometryChanged();
	emit should_repaint();
	
}
void CanvasElement::select(){ isSelected_ = true; }
void CanvasElement::deselect(){ isSelected_ = false;}
bool CanvasElement::isSelected(){ return isSelected_;}
void CanvasElement::dragSelected(int canvas_x, int canvas_y){
	(void) canvas_x;
	(void) canvas_y;
	qDebug() << "CanvasElementWidget::dragSelected is NOT IMPLEMENTED YET" << '\n';
}
void CanvasElement::stopDrag(QPoint cPos){
	stopDragOverrideThis(cPos);
	isDragging = false;
	lastDragPoint = cPos;
}

std::weak_ptr<Connector> CanvasElement::tryConnToThis(std::weak_ptr<CanvasElement> srcOwner, QPoint pOnCanv, std::weak_ptr<Plug> plug){
	auto observedPlug = plug.lock();
	if(!observedPlug)
		return std::shared_ptr<Connector>();
	auto srcObserved = srcOwner.lock();
	if(srcObserved == nullptr)
		return std::shared_ptr<Connector>();
	for(auto& it : getConnectors()){
		if(!it.lock())
			continue;
		if(it.lock()->getGeometry().contains(pOnCanv) 
				&& it.lock()->getConnectableCETypes().count(srcObserved->CEtype)){
			auto prevConnectedConn = observedPlug.get()->getConnectedConn();
			observedPlug.get()->detach();
			observedPlug.get()->attach(*it.lock());
			
			srcObserved->connectionAllowed(weak_from_this(), it, pOnCanv, plug);
			return it;
		}
	}
	return std::shared_ptr<Connector>();
}
void CanvasElement::connectionAllowed(std::weak_ptr<CanvasElement> ceT, 
							std::weak_ptr<Connector> connT, 
							QPoint, 
							std::weak_ptr<Plug> plug){
	emit ConnectedTo_C(weak_from_this(), plug, ceT, connT);
}

std::weak_ptr<Connector> CanvasElement::mouseDoublePressed_My(Target t){
	auto targetCEO = t.canvasElement.lock();///TODO: ugyis t.canvasElement.lock()-nak hivom meg ezt a fuggvenyet, ezert nem kell megkülönböztetni shared_from_this-től. a kettő ugynaz lesz.
	auto ceo = shared_from_this();
	assert(targetCEO);
	assert(ceo);
	assert(targetCEO.get() == ceo.get());
	auto pressedConns = getConnFirstAtCanvasCoords(
				QPoint(t.x_inCanvasOriginal, t.y_inCanvasOriginal));
	//auto pressedConns = getConnsAllAtCanvasCoords(
	//			QPoint(t.x_inCanvasOriginal, t.y_inCanvasOriginal));
	return pressedConns;
	//emit mouseDoublePressedSig(t);
}
	   
void CanvasElement::copyFrom(const CanvasElement* o){
	/// nem masolom isSelected_-t
	/// nem masolom a CEtype-t
	/// nem masolom a plugs-t
	geometry = o->geometry;
	/// nem masolom a dragStartX-t
	/// nem masolom a dragStartY-t
	lastDragPoint = o->lastDragPoint;
	/// nem masolom a isDragging-t
	/// nem masolom a connectors-t
}
void CanvasElement::CEW_setGeometry(QRect r){
	geometry = r;
}
void CanvasElement::connectorTranslationChanged(std::weak_ptr<Connector> srcConn, std::weak_ptr<Plug> srcPlug, QPoint newTr){
	(void) srcConn;
	(void) srcPlug;
	//(void) prevTr;
	(void) newTr;
}
void CanvasElement::connectorDirectionChanged(std::weak_ptr<Connector> srcConn, std::weak_ptr<Plug> srcPlug, Direction newDir){
	(void) srcConn;
	(void) srcPlug;
	//(void) prevDir;
	(void) newDir;
}
void CanvasElement::connectorAreaChanged(std::weak_ptr<Connector> srcConn, std::weak_ptr<Plug> srcPlug, QRect newArea){
	(void) srcConn;
	(void) srcPlug;
	(void) newArea;
}
