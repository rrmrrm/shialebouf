#ifndef PLUG_H
#define PLUG_H

#include "model/dirDim.hpp"
class Connector;
//#include "model/AggregationSaver/ihasaggregate.h"
#include "model/aggregationt.h"

#include <QObject>
#include <QRect>
class CanvasElement;
#include <memory>

class Plug : 
		public QObject, 
		//public virtual IHasAggregate,
		public virtual Identifiable, 
		public std::enable_shared_from_this<Plug>
{
	Q_OBJECT
protected:
	AggregationT<Connector> connectedConn;
	//uint id_inCE;
	CanvasElement * const containingCE;
public:
	Plug();
	Plug(CanvasElement*);
	//Plug(CanvasElement*, uint id_);
	virtual ~Plug();
	
public:
	std::ostream& saveAggregations(std::ostream&);
	std::istream& loadAggregations(std::istream&);
	virtual std::ostream& save(std::ostream&) override;
	virtual std::istream& load(std::istream&) override;
	//void setId(uint id_);
	//uint getId();
	QRect getGeometry() const;
	std::weak_ptr<Connector> getConnectedConn() const;
	void attach(Connector& conn);
	//detach(): connectedConn felé megszakítom a signal-slot kapcsolatokat
	void detach();
	CanvasElement* getContainingCE();
public slots:
	void connectorTranslationChanged(std::weak_ptr<Connector>src, QPoint newTr);
	void connectorDirectionChanged(std::weak_ptr<Connector>src, Direction newDir);
	void connectorAreaChanged(std::weak_ptr<Connector>, QRect newArea);
//signals:
//	void translationChangedCalled(std::weak_ptr<Connector>, std::weak_ptr<Plug>, QPoint newTr);
//	void directionChangedCalled(std::weak_ptr<Connector>, std::weak_ptr<Plug>, Direction newDir);
//	void areaChangedCalled(std::weak_ptr<Connector>, std::weak_ptr<Plug>, QRect newArea);
};


#endif // PLUG_H
