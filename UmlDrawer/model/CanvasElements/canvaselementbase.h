#ifndef CANVASELEMENTBASE_H
#define CANVASELEMENTBASE_H

#include "canvaselementtypes.h"
#include "shape_drawing/drawing.h"
#include "model/dirDim.hpp"
#include "model/identityDir/identifiable.h"
#include "model/aggregationt.h"
#include "compositiont.h"

#include <QDebug>
#include <QMouseEvent>

#include <memory>
#include <vector>

class Plug;
class Connector;
class Target;
//using AggregationT = std::weak_ptr<AggregatedT>;

//template<class AggregatedT>
//using AggregationT = AggregationT<AggregatedT>;

class CanvasElement;
//typedef MyCompositionT<CanvasElement> CanvasElementContainerT;

class CanvasElement: 
		public QObject,
		//public virtual IHasAggregate,
		public virtual Identifiable, 
		public std::enable_shared_from_this<CanvasElement>
{
	Q_OBJECT
protected:
	bool isSelected_ = false;
	///TODO: lehetne itt shared_ptr /eket tartolni,
	///  es a lesytramyaottbanc typedeffel hivatkoyni ay egzes elemekre,
	///  hisy ugzse valtoynak a connector-ok es plug-ok
	std::vector<AggregationT<Connector> > connectors;
	Model* model;
public:
	const CanvasElement_Type CEtype;
	std::vector<AggregationT<Plug> > plugs;
	QRect geometry;
	int dragStartX = 0;
	int dragStartY = 0;
	QPoint lastDragPoint;
	bool isDragging = false;
protected:
	CanvasElement(Model* model_, CanvasElement_Type type_, QRect geometry_);
	virtual ~CanvasElement();
	
protected:
	void moveTopLeft(int cX, int cY);
	//void forwardPlugSig(Plug* pl);
	virtual void dragUnselectedOverrideThis(int canvas_x, int canvas_y);
	virtual void stopDragOverrideThis(QPoint cPos);
public:
	static MyCompositionT<CanvasElement> createCanvasElement(Model* model, CanvasElement_Type ce_type, QRect area);
	virtual std::ostream& saveAggregations(std::ostream&);
	virtual std::istream& loadAggregations(std::istream&);
	virtual std::ostream& save(std::ostream&) override;
	virtual std::istream& load(std::istream&) override;
	/// a Canvas coordinátarendszerében értendő a visszaadott QRect:
	QRect getGeometry();
	//std::weak_ptr<Plug> getPlugAt(QPoint p);
	std::weak_ptr<Connector> getConnAt(QPoint p);
	//std::weak_ptr<Plug> getPlugAt(int cX, int cY);
	std::weak_ptr<Connector> getConnAt(int cX, int cY);
	virtual std::weak_ptr<Plug> getMainPlug() const = 0;
	virtual std::vector<AggregationT<Connector>> getConnectors();
	virtual bool containsPoint(QPoint p) const;
	/*virtual*/ std::weak_ptr<Connector> getConnFirstAtCanvasCoords(QPoint);
	virtual std::vector<AggregationT<Connector>> getConnsAllAtCanvasCoords(QPoint);
	virtual int x() const ;
	virtual int y() const ;
	virtual int width() const;
	virtual int height() const;
	//virtual void setGeometrySizeCentered(QSize newSize);
	void dragUnselected(int canvas_x, int canvas_y);
	void select();
	void deselect();
	bool isSelected();
	virtual void dragSelected(int canvas_x, int canvas_y);
	void stopDrag(QPoint cPos);
	
	virtual std::weak_ptr<Connector> tryConnToThis(std::weak_ptr<CanvasElement> srcOwner, QPoint pOnCanv, std::weak_ptr<Plug> plug);

	virtual void connectionAllowed(std::weak_ptr<CanvasElement> src, 
								   std::weak_ptr<Connector> connSrc, 
								   QPoint p, 
								   std::weak_ptr<Plug> plug);
	std::weak_ptr<Connector> mouseDoublePressed_My(Target t);
	/// megjegzes: Identifiable és  QObject nem másolható.
	virtual void copyFrom(const CanvasElement* o);
signals:
	void should_repaint();
	void geometryChanged_CEW();
	void drag_CEW(int cX, int cY);
	void resizeEdgeSig(int cX, int cY);

	void ConnectedTo_C(
			std::weak_ptr<CanvasElement> ceS,
			std::weak_ptr<Plug> pl,
			std::weak_ptr<CanvasElement> ceT, 
			std::weak_ptr<Connector> conn
	);
public slots:
	/// r-t a Canvas coordrendszerében várjuk
	void CEW_setGeometry(QRect r);
	
	virtual void connectorTranslationChanged(std::weak_ptr<Connector> srcConn, std::weak_ptr<Plug> srcPlug, QPoint newTr);
	virtual void connectorDirectionChanged(std::weak_ptr<Connector> srcConn, std::weak_ptr<Plug> srcPlug, Direction newDir);
	virtual void connectorAreaChanged(std::weak_ptr<Connector> srcConn, std::weak_ptr<Plug> srcPlug, QRect newArea);

};
#endif // CANVASELEMENTBASE_H
