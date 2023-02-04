#include "connector.h"
#include "model/plug.h"
#include "persistence/streamoperators.h"
#include <set>
#include <qdebug.h>


Connector::Connector(CanvasElement* containingCE_, std::set<CanvasElement_Type> cts_)
	: connectableCETypes(cts_)
	, containingCE(containingCE_)
{
	assert(connectableCETypes.size());
	assert(containingCE_);
}
Connector::Connector(CanvasElement* containingCE_, CanvasElement_Type ct)
  : Connector(containingCE_, std::set<CanvasElement_Type>{ct})
{}
//Connector::Connector(CanvasElement* containingCE_, QRect geometry_, CanvasElement_Type ct):
//	connectableCETypes({ct}),
//	geometry(geometry_),
//	containingCE(containingCE_)
//{
//	assert(connectableCETypes.size());
//	assert(containingCE_);
//}
Connector::Connector(CanvasElement* containingCE_, Direction dir_, CanvasElement_Type ct):
	connectableCETypes({ct}),
	dir(dir_),
	containingCE(containingCE_)
{
	assert(connectableCETypes.size());
	assert(containingCE_);
}
Connector::~Connector(){
}


std::ostream& Connector::save(std::ostream& os){
	Identifiable::save(os);
	// connectableCETypes konstans
	os << dir << std::endl;
	os << geometry;
	bool backwardsCompatibleDummy = 0 ; 
  os << backwardsCompatibleDummy  << std::endl;
	return os;
}

std::istream& Connector::load(std::istream& is){
	Identifiable::load(is);
	// connectableCETypes konstans
	int idir = 0; is >> idir; assert(is); dir = (Direction)idir;
	is >> geometry;
  bool backwardsCompatibleDummy = 0 ; 
	is >> backwardsCompatibleDummy;
	return is;
}

QRect Connector::getGeometry() const{
	return geometry;
}

std::ostream& Connector::saveAggregations(std::ostream& os){
	saveContainer(os, connectedPlugs);
	return os;
}
std::istream& Connector::loadAggregations(std::istream& is){
	loadContainer(is, connectedPlugs);
	return is;
}

std::set<CanvasElement_Type> Connector::getConnectableCETypes() const{
	return connectableCETypes;
}
Direction Connector::getDir() const{
	return dir;
}
bool Connector::containsPoint(QPoint p) const{
	return geometry.contains(p);
}
void Connector::setDir(Direction dir_){
	dir = dir_;
	emit directionChanged(weak_from_this(), dir);
}

void Connector::CW_setGeometry(QRect r){
	geometry = r;
  emit translationChanged(weak_from_this(), r.topLeft());
	emit areaChanged(weak_from_this(), r);
}
