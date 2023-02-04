#include "plug.h"
#include "connector.h"
#include "model/CanvasElements/canvaselementbase.h"

Plug::Plug():containingCE(nullptr)
{
	qDebug() << "warning: Plug::Plug(): empty ctor called containingCE will be nullptr\n";
}
Plug::Plug(CanvasElement* cceb) : containingCE(cceb){
}
Plug::~Plug(){
	detach();
}


std::ostream& Plug::save(std::ostream& os){
	Identifiable::save(os);
	//os << id_inCE << std::endl;
	return os;
}

std::istream& Plug::load(std::istream& is){
	Identifiable::load(is);
	//is >> id_inCE;
	return is;
}
std::ostream& Plug::saveAggregations(std::ostream& os){
	//base::saveAggsChainCallThis(os);
	connectedConn.save(os);
	return os;
}

std::istream& Plug::loadAggregations(std::istream& is){
	//base::loadAggsFinalizeChainCallThis(is);
	connectedConn.load(is, this);
	return is;
}


std::weak_ptr<Connector> Plug::getConnectedConn() const{
	return connectedConn;
}
void Plug::attach(Connector& conn){
	connectedConn = conn.weak_from_this();
	auto obs = connectedConn.lock();
	obs->connectedPlugs.push_back(weak_from_this());
	//modelConnect
  QObject::connect(obs.get(),
		SIGNAL(translationChanged(std::weak_ptr<Connector>,  QPoint)),
		this,
		SLOT(connectorTranslationChanged(std::weak_ptr<Connector>, QPoint))
	);
  //modelConnect
  QObject::connect(obs.get(),
		SIGNAL(directionChanged(std::weak_ptr<Connector>, Direction)),
		this,
		SLOT(connectorDirectionChanged(std::weak_ptr<Connector>, Direction))
	);
  //modelConnect
  QObject::connect(obs.get(),
		SIGNAL(areaChanged(std::weak_ptr<Connector>, QRect)),
		this,
		SLOT(connectorAreaChanged(std::weak_ptr<Connector>, QRect))
	);
}
void Plug::detach(){
	if(auto obs = connectedConn.lock()){
		disconnect(obs.get(),
			&Connector::translationChanged,
			this,
			&Plug::connectorTranslationChanged
		);
		disconnect(obs.get(),
			&Connector::directionChanged,
			this,
			&Plug::connectorDirectionChanged
		);
		disconnect(obs.get(),
			&Connector::areaChanged,
			this,
			&Plug::connectorAreaChanged
		);
		/// a csatlakozott Connector connectedPlugs vektorából törlöm ezt a Plug-ot
		auto found = std::find_if(
					obs->connectedPlugs.begin(), 
					obs->connectedPlugs.end(),
					[this](AggregationT<Plug>& plug ){
						if(auto pobs = plug.lock())
							return this == pobs.get();
						return false;
					}
		);
		if(found != obs->connectedPlugs.end()){
			obs->connectedPlugs.erase(found);
		}
	}
  connectedConn.reset();
}

CanvasElement* Plug::getContainingCE(){
	return containingCE;
}


void Plug::connectorTranslationChanged(std::weak_ptr<Connector> srcConn, QPoint newTr){
  if(!getContainingCE())
    return;
  getContainingCE()->connectorTranslationChanged(srcConn, weak_from_this(), newTr);
	//emit translationChangedCalled(srcConn, weak_from_this(), newTr);
}
void Plug::connectorDirectionChanged(std::weak_ptr<Connector>srcConn, Direction newDir){
  if(!getContainingCE())
    return;
  getContainingCE()->connectorDirectionChanged(srcConn, weak_from_this(), newDir);
	//emit directionChangedCalled(srcConn, weak_from_this(), newDir);
}
void Plug::connectorAreaChanged(std::weak_ptr<Connector> srcConn, QRect newArea){
  if(!getContainingCE())
    return;
  getContainingCE()->connectorAreaChanged(srcConn, weak_from_this(), newArea);
	//emit areaChangedCalled(srcConn, weak_from_this(), newArea);
}
