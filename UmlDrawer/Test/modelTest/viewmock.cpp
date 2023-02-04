#include "viewmock.h"
#include "model/model.h"
#include <QWidget>
ViewMock::ViewMock(Model* m_)
	: m(m_)
	, canvas(new CanvasMock)
{
	
	connect(m, SIGNAL(updateCanvas(QRect)), canvas, SLOT(Update(QRect)));
	connect(m, SIGNAL(updateCanvas()), canvas, SLOT(Update()));
	connect(m, SIGNAL(destroyView_W(IdType)), canvas, SLOT(M_destroyCEW(IdType)));
	connect(
		m,
		SIGNAL(askForUserInput(std::shared_ptr<UserInputModel>, std::function<void (std::shared_ptr<UserInputModel>)>)),
		this,
		SLOT(askForUserInput(std::shared_ptr<UserInputModel>, std::function<void (std::shared_ptr<UserInputModel>)>))
	);
	connect(m, SIGNAL(showMessage(QString)),this, SLOT(showMessage(QString)));
	connect(
		m, 
		SIGNAL(addDotMatrixMiniature(DotMatrix, int)), 
		this, 
		SLOT(addDotMatrixMiniature(DotMatrix, int))
	);
	connect(
		m, 
		SIGNAL(removeDotMatrixMiniature(int)), 
		this, 
		SLOT(removeDotMatrixMiniature(int))
	);
	connect(
		m, 
		SIGNAL(removeDrawingDotMatrixMiniatures()), 
		this, 
		SLOT(removeDrawingDotMatrixMiniatures())
	);
	connect(
		m, 
		SIGNAL(canvasElementAdded(
				   std::weak_ptr<CanvasElement>
			   )
		), 
		canvas, 
		SLOT(Model_CanvasElementAdded(
				std::weak_ptr<CanvasElement>
			)
		)
	);
	connect(
		m,
		SIGNAL(removeCanvasElements()),
		canvas,
		SLOT(removeCanvasElementWidgets())
	);
}

void ViewMock::askForUserInput(std::shared_ptr<UserInputModel>, std::function<void (std::shared_ptr<UserInputModel>)> callback){}
void ViewMock::addDotMatrixMiniature(DotMatrix drawing, int pos_canBeOverIndexed){
  drawMat = drawing;
  ++drawMatMiniCalledTimes;
}
void ViewMock::removeDotMatrixMiniature(int pos_canBeOverIndexed){}
void ViewMock::updateDrawingDotMatrixMiniatures(){}
void ViewMock::messagePopup(std::string msg){}
void ViewMock::showNeuralNetworkResult(std::vector<double> result){}
void ViewMock::prepareForClosing(){}
void ViewMock::showMessage(QString msg){
  showMessageCalled = true;
}
CanvasMock::CanvasMock(){

}
void CanvasMock::Model_CanvasElementAdded(std::weak_ptr<CanvasElement> ce){}
void CanvasMock::M_destroyCEW(IdType){}
void CanvasMock::CE_ConnectedTo(std::weak_ptr<CanvasElement> ceS,
			 std::weak_ptr<Plug> pl,
			 std::weak_ptr<CanvasElement> ceT, 
			 std::weak_ptr<Connector> conn){}
void CanvasMock::removeCanvasElementWidgets(){}
void CanvasMock::Update(QRect){}
void CanvasMock::Update(){}
void CanvasMock::saveAsImage(QString file){}
std::weak_ptr<CanvasElement> CanvasMock::srcAsCE(){}
QPoint CanvasMock::pToCanv(QMouseEvent* event){}
void CanvasMock::EntityWidgetmousePressEvent(QMouseEvent *event){} 
void CanvasMock::EntityWidgetmouseMoveEvent(QMouseEvent *event){}
void CanvasMock::EntityWidgetmouseReleaseEvent(QMouseEvent *event){}
void CanvasMock::EntityWidgetmouseDoubleClickEvent(QMouseEvent *event){}
