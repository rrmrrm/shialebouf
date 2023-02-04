#ifndef VIEWMOCK_H
#define VIEWMOCK_H
#include "common.hpp"
#include "model/model.h"
#include <QWidget>
class CanvasMock;
class ViewMock : public QObject
{
	Q_OBJECT
public:
	Model* m = nullptr;
	CanvasMock* canvas;
  int drawMatMiniCalledTimes = 0;
  DotMatrix drawMat;
  bool showMessageCalled = false;
	ViewMock(Model* m_);
	
public slots:
	void askForUserInput(std::shared_ptr<UserInputModel>, std::function<void (std::shared_ptr<UserInputModel>)> callback);

	void addDotMatrixMiniature(DotMatrix drawing, int pos_canBeOverIndexed);
	void removeDotMatrixMiniature(int pos_canBeOverIndexed);
	void updateDrawingDotMatrixMiniatures();
	void messagePopup(std::string msg);
	void showNeuralNetworkResult(std::vector<double> result);
	void prepareForClosing();
  void showMessage(QString);
	
};
class CanvasMock : public QObject 
{
	Q_OBJECT
public:
	CanvasMock();
public slots:
	void Model_CanvasElementAdded(std::weak_ptr<CanvasElement> ce);
	void M_destroyCEW(IdType);
	void CE_ConnectedTo(std::weak_ptr<CanvasElement> ceS,
					 std::weak_ptr<Plug> pl,
					 std::weak_ptr<CanvasElement> ceT, 
					 std::weak_ptr<Connector> conn);
	void removeCanvasElementWidgets();
	void Update(QRect);
	void Update();
	void saveAsImage(QString file);
	std::weak_ptr<CanvasElement> srcAsCE();
	QPoint pToCanv(QMouseEvent* event);
	void EntityWidgetmousePressEvent(QMouseEvent *event) ;
	void EntityWidgetmouseMoveEvent(QMouseEvent *event);
	void EntityWidgetmouseReleaseEvent(QMouseEvent *event);
	void EntityWidgetmouseDoubleClickEvent(QMouseEvent *event);
};

#endif // VIEWMOCK_H
