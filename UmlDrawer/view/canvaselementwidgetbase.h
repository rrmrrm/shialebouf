#ifndef CANVASELEMENT_H
#define CANVASELEMENT_H

//#include "model/CanvasElements/canvaselementbase.h"
//#include "model/target.h"
#include <QWidget>
#include <QLayout>

class CanvasElement;
class ConnWidget;
class Connector;
class Plug;
class Canvas;

class CanvasElementWidget : public QWidget{
	Q_OBJECT
	
	//int permittedRepaintNum = 0;
	//QRect rectToRepaint;
protected:
  const std::weak_ptr<CanvasElement> ce;
	
public:
	CanvasElementWidget(std::weak_ptr<CanvasElement> ce_, QWidget* parent);
	virtual ~CanvasElementWidget();
  
  virtual void paintSelectionoutline();
protected:
	QRect toWCoords(QRect rInCanvas) const;
	/// Canvas-hoz relatív geometria beállítása:
	void setGeometryInC(QRect r);
public:
	/// Canvas-hoz relatív geometria lekérdezése:
	QRect getGeomInC();
	std::weak_ptr<CanvasElement> getCE() const;
	Canvas* accessCanvas();
	// ///ezzel a függvény döntjük el, hogy a smartUpdate(rect) hatására érkező paintEvent-ben újra kell-e rajzolni bármit is
	// bool shouldUsePaintEvent() const;
	// void updateCEGeom();
	// void updateConnsGeom();
  
  // textWidget használja, hogy az őtt tartalmazó Connector Widget kövesse a TextWidgetben levő szöveg méretét
	void ConnW_wantsToResize(ConnWidget* cw, QSize newSize);
	/// a CEW csatlakoztatása egy ConnWidget-hez:
	void C_connectedTo(std::weak_ptr<Plug> pl, CanvasElementWidget* cew, ConnWidget* conn);
  
  // helper for highlighting a the connWidget under the cursor (see Canvas)
  // returns connector widget at canvasPoint or nullptr
  ConnWidget* getConnWidgetAt(QPoint canvasPoint); 
private:
	//void decrPermRepaintNum();
  /// ha a CEW átméreteződik, akkor ez a slot meghívódik 
  /// frissíti a CE geometriáját és  meghívja a ConnWidgetek resizeEvent-jét:
	void resizeEvent(QResizeEvent *) override;
  /// ha a CEW elmozdul, akkor ez a slot meghívódik
	/// frissíti a CE geometriáját és  meghívja a ConnWidgetek resizeEvent-jét:
	void moveEvent(QMoveEvent *event) override;	
	
public slots:
  //void smartUpdate(QRect);
	//void smartUpdate();
  // myRepaint újrarajlzálja a CEW-et
  void myRepaint();
  //nem akarom, hogy a leszármazó osztályok a Qt paintEventjét felülírják és használják.
	// helyette a derivedPaintEvent-et kell felülírniuk
  // paintEvent megjelen<ti a kijel0lt elem keretét, és meghívja derivedPaintEvent-et
	void paintEvent(QPaintEvent* e) final;
  virtual void derivedPaintEvent(QPaintEvent* e);
	
	void CE_Drag(int cX, int cY);
	//virtual void CE_getPlugAtCCoords(int cX, int cY);
	void CE_resize(int cX, int cY/*, Direction draggedEdge*/);	
  virtual void CE_geometryChanged();	
  // egéresemény feldolgozására
	virtual void mousePressEvent(QMouseEvent *event) override;
	virtual void mouseMoveEvent(QMouseEvent *event) override;
	virtual void mouseReleaseEvent(QMouseEvent *event) override;
signals:
	void setGeometry_CEB(QRect r);
	//erre a widgetre érkező egéreseményt továbbítja a canvasnak
	void mousePressEventAcc_C(QMouseEvent *event);
	void mouseMoveEventAcc_C(QMouseEvent *event);
	void mouseReleaseEventAcc_C(QMouseEvent *event);
	//void returnPlugAtCCoords_CE(std::weak_ptr<Plug>);
};


#endif // CANVASELEMENT_H
