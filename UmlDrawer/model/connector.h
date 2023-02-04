#ifndef CONNECTOR_H
#define CONNECTOR_H

#include "CanvasElements/canvaselementtypes.h"
#include "dirDim.hpp"
#include "identityDir/identifiable.h"
//#include "model/AggregationSaver/ihasaggregate.h"
#include "model/aggregationt.h"

#include <set>
#include <memory>
class CanvasElement;
class Plug;

class Connector : 
		public QObject, 
		public virtual Identifiable, 
		//public virtual IHasAggregate, 
		public std::enable_shared_from_this<Connector>
{
	Q_OBJECT
private:
	const std::set<CanvasElement_Type> connectableCETypes;
	Direction dir = Direction::RIGHT;
	/// megjegyzés: a canvashoz képest relatívak a Connector koordinátái, nem a tartalmazó Canvaselementhez
	QRect geometry;
protected:
  // // TODO  már nem kell	
	// bool movedAway = false;
public:
	/// a felcsatlakoztatott CE-k eléréséhez:
	std::vector<AggregationT<Plug>> connectedPlugs;
	CanvasElement* const containingCE;
	
public:
	Connector(CanvasElement* containingCE_, std::set<CanvasElement_Type> cts);
	Connector(CanvasElement* containingCE_, CanvasElement_Type ct);
	//Connector(CanvasElement* containingCE_, QRect geometry_, CanvasElement_Type ct);
	Connector(CanvasElement* containingCE_, Direction dir_, CanvasElement_Type ct);
	virtual ~Connector();

public:
	std::ostream& saveAggregations(std::ostream&);
	std::istream& loadAggregations(std::istream&);
	virtual std::ostream& save(std::ostream&) override;
	virtual std::istream& load(std::istream&) override;
	QRect getGeometry() const;
	std::set<CanvasElement_Type> getConnectableCETypes() const;
	Direction getDir() const;
	bool containsPoint(QPoint p) const;
	void setDir(Direction dir);
public slots:
	void CW_setGeometry(QRect);
signals:
	/// ezek a felcsatlakozott Plug-oknak szólnak. 
	/// A connect meghívását a Plug osztály végzi el:
	void translationChanged(std::weak_ptr<Connector>, QPoint newTr);
	void directionChanged(std::weak_ptr<Connector>, Direction newDir);
	void areaChanged(std::weak_ptr<Connector>, QRect newArea);
	
};

#endif // CONNECTOR_H
