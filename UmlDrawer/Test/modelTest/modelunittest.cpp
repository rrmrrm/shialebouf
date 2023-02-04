#include "modelunittest.h"
#include <QtTest/QtTest>
#include "model/model.h"
#include "common.hpp"
#include "viewmock.h"
//#include <QSignalSpy>
#include <fstream>
QTEST_MAIN(Unittest::ModelUnittest);
namespace{
void makeDefConfOs(int resHor, int resVert, int marg, int drawB, bool testNetwork, std::string doutfname){
  std::ofstream confOs(CONF_FILE_NAME);
  confOs << "resHorizontal :" << std::endl;
  confOs << resHor << std::endl;
  confOs << "resVertical :" << std::endl;
  confOs << resVert << std::endl;
  confOs << "marginInPixels :" << std::endl;
  confOs << marg << std::endl;
  confOs << "drawingBlurSD :" << std::endl;
  confOs << drawB << std::endl;
  confOs << "testNetwork :" << std::endl;
  confOs << testNetwork << std::endl;
  confOs << "drawingsOutFileName :" << std::endl;
  confOs << doutfname << std::endl;
  if(!confOs.is_open() || confOs.fail()){
    exit(2);
  }
  confOs.close();
}
struct DrawingCLassifierMock: public IDrawingClassifier{
  bool classifyCalled = false;
  ~DrawingCLassifierMock()
  {}
  virtual CanvasElement_Type classify(Drawing* d){
    classifyCalled = true;
  }
};
}

namespace Unittest{
ModelUnittest::ModelUnittest()
{
	
}
void ModelUnittest::initTestCase()
{
	
}
void ModelUnittest::cleanupTestCase()
{
}
void ModelUnittest::init(){

}
void ModelUnittest::cleanup(){

}

void ModelUnittest::ctorTest1()
{
  makeDefConfOs(30, 40, 4, 4, 0, "drawingsOut.txt");
  Model* model = new Model();
  QCOMPARE(model->resHorizontal, 30);
  QCOMPARE(model->resVertical, 40);
  QCOMPARE(model->testNetwork, 0);
  delete model;
}

void ModelUnittest::ctorTest2()
{
  makeDefConfOs(30, 40, 4, 4, 1, "drawingsOut.txt");
  Model* model = new Model();
	QVERIFY(model->dc != nullptr);
  delete model;
}
void ModelUnittest::ctorTest3()
{
  makeDefConfOs(30, 40, 4, 4, 0, "drawingsOut.txt");
  Model* model = new Model();
	QVERIFY(model->lastDrawing != nullptr);
  delete model;
}
void ModelUnittest::dtorTest1()
{
  makeDefConfOs(30, 40, 4, 4, 0, "drawingsOut.txt");
  Model* model = new Model();
  int refCnt0 = Persistence::refCounter;
  QVERIFY(refCnt0>0);
  delete model;
  int refCnt1 = Persistence::refCounter;
  QVERIFY(refCnt1+1 == refCnt0);
  
}
void ModelUnittest::ddamTest1(){
  makeDefConfOs(30, 40, 4, 4, 0, "drawingsOut.txt");
  Model* model = new Model();
  model->drawings.push_back(new Drawing());
  QCOMPARE(model->drawings.size(), 1); // teszteles elofeltetele - ha nem teljesul, a teszt rossz, nem a tesztelt cucc
  
  {
  auto viewMock = new ViewMock(model);
  QCOMPARE(viewMock->drawMatMiniCalledTimes, 0); // teszteles elofeltetle
  model->displayDrawingsAsMiniatures();
  QVERIFY(viewMock->drawMatMiniCalledTimes >= 1); // el kellett kuldenie a addDotMatrixMiniature jelet
  delete viewMock;
  }
  delete model;
}
void ModelUnittest::ddamTest2(){
  makeDefConfOs(30, 40, 4, 4, 0, "drawingsOut.txt");
  Model* model = new Model();
  QCOMPARE(model->drawings.size(), 0); // teszteles elofeltetele
  {
  auto viewMock = new ViewMock(model);
  QCOMPARE(viewMock->drawMatMiniCalledTimes, 0);// teszteles elofeltetle
  model->displayDrawingsAsMiniatures();
  QCOMPARE(viewMock->drawMatMiniCalledTimes, 0); // nem syabad elkuldenie a addDotMatrixMiniature jelet
  delete viewMock;
  }
  delete model;
}
void ModelUnittest::ddamTest3(){
  makeDefConfOs(30, 40, 4, 4, 0, "drawingsOut.txt");
  Model* model = new Model();
  {
  model->drawings.push_back(new Drawing());
  model->drawings.push_back(new Drawing());
  model->drawings.push_back(new Drawing());  
  QCOMPARE(model->drawings.size(), 3); // teszteles elofeltetele
  auto viewMock = new ViewMock(model);
  QCOMPARE(viewMock->drawMatMiniCalledTimes, 0);// teszteles elofeltetle
  model->displayDrawingsAsMiniatures();
  QCOMPARE(viewMock->drawMatMiniCalledTimes, 3);
  delete viewMock;
  }
  delete model;
}
void ModelUnittest::ddamTest4(){
  makeDefConfOs(10, 10, 4, 4, 0, "drawingsOut.txt");
  Model* model = new Model();
  auto d= new Drawing(1,1 , 10, 10);
  d->addGesture(Gesture());
  d->addGesturePoint(1,1);
  d->addGesturePoint(2,2);
  d->addGesturePoint(3,3);
  d->addGesturePoint(4,4);
  d->addGesturePoint(5,5);
  d->updatePixelMatrix(true); // d pixelmatrixanak elkeszitese
  model->drawings.push_back(d);
  QCOMPARE(model->drawings.size(), 1); // teszteles elofeltetele 
  
  {
  auto viewMock = new ViewMock(model);
  QCOMPARE(viewMock->drawMatMiniCalledTimes, 0);// teszteles elofeltetle
  QVERIFY(viewMock->drawMat != d->getPixelMatrix());
  model->displayDrawingsAsMiniatures();
  QCOMPARE(viewMock->drawMat, d->getPixelMatrix());
  
  }
  delete model;
}
void ModelUnittest::gldTest(){
  makeDefConfOs(10, 10, 4, 4, 0, "drawingsOut.txt");
  Model* model = new Model();
  auto expected = model->lastDrawing;
  auto actual = model->getLastDrawing();
  QCOMPARE(actual, expected);
  delete model;
}
void ModelUnittest::indTest1(){
  makeDefConfOs(10, 10, 4, 4, 0, "drawingsOut.txt");
  Model* model = new Model();
  auto ld = model->lastDrawing;
  model->initNewDrawing();
  auto newD = model->lastDrawing;
  QVERIFY(ld != newD);
  delete model;
}
void ModelUnittest::indTest2(){
  makeDefConfOs(10, 15, 4, 4, 0, "drawingsOut.txt");
  Model* model = new Model();
  auto ld = model->lastDrawing;
  model->initNewDrawing();
  auto newD = model->lastDrawing;
  QVERIFY(newD != nullptr);
  QCOMPARE(newD->getResolutionHorizontal(), 10);
  QCOMPARE(newD->getResolutionVertical(), 15);
  delete model;
}
void ModelUnittest::prrldTest1(){
  makeDefConfOs(10, 15, 4, 4, 0, "drawingsOut.txt");
  Model* model = new Model();
  model->lastDrawing->accessGestures().clear();
  QCOMPARE(model->lastDrawing->accessGestures().size(), 0); //teszt ef.
  auto viewMock = new ViewMock(model);
  QVERIFY(!viewMock->showMessageCalled); // teszt ef.
  model->pushRecoginzeReplaceLastDrawing();
  QVERIFY(viewMock->showMessageCalled);
  delete viewMock;
  delete model;
}
void ModelUnittest::prrldTest2(){
  makeDefConfOs(10, 15, 4, 4, 0, "drawingsOut.txt");
  Model* model = new Model();
  QVERIFY(model->lastDrawing->accessGestures().size() != 0); //teszt ef.
  auto viewMock = new ViewMock(model);
  auto ld = model->lastDrawing;
  QVERIFY(std::find(model->getDrawings().begin(), model->getDrawings().end(), ld) == model->getDrawings().end()); // teszt ef.
  model->pushRecoginzeReplaceLastDrawing();
  QVERIFY(std::find(model->getDrawings().begin(), model->getDrawings().end(), ld) != model->getDrawings().end());
  
  delete viewMock;
  delete model;
}
void ModelUnittest::prrldTest3(){
  makeDefConfOs(10, 15, 4, 4, 1, "drawingsOut.txt");
  Model* model = new Model();
  QVERIFY(model->testNetwork); // ef.
  //lecserelem dc-t a tesztelo implementaciora
  delete model->dc;
  auto dcm = new DrawingCLassifierMock;
  model->dc = dcm;
  QVERIFY(!dcm->classifyCalled); // ef.
  model->pushRecoginzeReplaceLastDrawing();
  QVERIFY(dcm->classifyCalled);
  delete model;
}
void ModelUnittest::prrldTest4(){
  makeDefConfOs(10, 15, 4, 4, 0, "drawingsOut.txt");
  Model* model = new Model();
  auto viewMock = new ViewMock(model);
  auto ld = model->lastDrawing;
  QVERIFY(std::find(model->getDrawings().begin(), model->getDrawings().end(), ld) == model->getDrawings().end()); // teszt ef.
  model->pushRecoginzeReplaceLastDrawing();
  QVERIFY(model->drawings.size() > 0);
  QCOMPARE(model->drawings.back(), ld);
  QVERIFY(model->lastDrawing != ld); // lastdrawing nemszabad, hogy az eloyo erteket megtartsa
  QVERIFY(model->lastDrawing != nullptr); // hanem egz uj elemre kell mutasson
  delete viewMock;
  delete model;
}
}
// this line is only needed if we have the testclass declaration as well as the implementation here in the .cpp file:
//#include "modelunittest.moc"
