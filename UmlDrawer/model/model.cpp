#include "model.h"
#include "network/network.h"
#include "CanvasElements/canvaselement.h"
#include "common.hpp"
#include "persistence/persistence.h"
#include "shape_drawing/drawing.h"
#include "model/plug.h"
#include <QMouseEvent>
#include <QDirIterator>
#include <QTimer>

#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <memory>
#include <stdlib.h>
#include <variant>
#include <string>
#include <cassert>
#include <iostream>
Model::Model():
		Identifiable(0),
		QObject(nullptr),
		saveLastDrawingModel(
			std::make_shared<GenericFileUIM>(
				this,
				"Utolsó elkészített ábra fileba mentése"))
{
	persistence = Persistence::GetInstance();
  
  persistence->loadConfig(
        std::vector<std::pair<QString, ptr_variants> >({
          std::make_pair(nameOf(resHorizontal), ptr_variants(&resHorizontal)),
          std::make_pair<QString, ptr_variants>(nameOf(resVertical), ptr_variants(&resVertical)),
          std::make_pair<QString, ptr_variants>(nameOf(marginInPixels), ptr_variants(&marginInPixels)),
          std::make_pair<QString, ptr_variants>(nameOf(drawingBlurSD), ptr_variants(&drawingBlurSD)),
          std::make_pair<QString, ptr_variants>(nameOf(testNetwork), ptr_variants(&testNetwork)),
          std::make_pair<QString, ptr_variants>(nameOf(drawingsOutFileName), ptr_variants(&drawingsOutFileName)),
        }),
        "Model::Model()"
      );
  if(testNetwork){
    dc = new NeuralClassifier(resHorizontal, resVertical);
  }
	//drawingFactory = new DrawingFactory(drawingBlurSD, marginInPixels);
  //lastDrawing = drawingFactory->create(resHorizontal, resVertical);
  lastDrawing = new Drawing(drawingBlurSD, marginInPixels, resHorizontal, resVertical);
	
	// /// bizonyos időközönként mentjük a teljes modell állapotot. 
	// QTimer *timer = new QTimer(this);
	// timer->setInterval(10*1000);
	// connect(timer, &QTimer::timeout, this, &Model::saveState);
	// timer->start();
}
Model::~Model(){
	if(dc)
    delete dc;
	persistence->releaseInstance();
}

void Model::displayDrawingsAsMiniatures(){
	int i = 0;
	for(auto d = drawings.begin() ; d != drawings.end() ; ++d, ++i){
		emit addDotMatrixMiniature((*d)->getPixelMatrix(), i);	
	}
}
// std::weak_ptr<CanvasElement> Model::getCEAt(QPoint cPos){
// 	for(const auto& ce : canvasElements){
// 		if(ce.get() && ce->getGeometry().contains(cPos)){
// 			return ce;
// 		}
// 	}
// 	return std::weak_ptr<CanvasElement>();
// }
// void Model::saveState(){
// 	stateManager.saveState();
// }
std::weak_ptr<GenericFileUIM> Model::getSaveLastDrawingModel(){
	return saveLastDrawingModel;
}
////todo: itt indexet is vissza kéne adni, vagy eltárolni inkább a sikeresen kijelolt Drawing indexét a drawings-ban:
//bool Model::selectDrawing(int x,int y){
//	bool found = false;
//	uint i = 0;
//	while(!found && i < drawings.size()){
//		if(!drawings[i]){
//			continue;
//		}
//		auto d = drawings[i];
//		//megnézzük, hogy az (x,y) pont benn van e
//		//a gesture-öket határoló téglalapban:
//		found = isPointInRectangle(
//					x,
//					y,
//					d->getMinX(), 
//					d->getMinY(), 
//					d->getMaxX() - d->getMinX(), 
//					d->getMaxY() - d->getMinY()
//				);
//		++i;
//	}
//	if(found){
//		--i;
//		
//	}
//	return found;
//}
const std::vector<Drawing*>& Model::getDrawings() const{
	return drawings;
}
Drawing const* Model::getLastDrawing() const {
	return lastDrawing;
}

///lastDrawing-ba új Drawing-ot készítünk:
void Model::initNewDrawing(){
  lastDrawing = new Drawing(drawingBlurSD, marginInPixels, resHorizontal, resVertical);
}
bool Model::addGestureToActiveDrawing(Gesture g){
	if(lastDrawing == nullptr){
		return false;
	}
	auto pre = lastDrawing->getGestures();
	lastDrawing->addGesture(g);
	emit updateCanvas(lastDrawing->getBoundingRect());
	return true;
}
void Model::addGesturePoint(int x, int y){
  if(!lastDrawing || lastDrawing->getGestures().size() == 0){
    persistence->accessLogger()->warn("Model::addGesturePoint(..): lastDrawing was null, or it had no gestures yet to add to");
    return;
  }
	lastDrawing->addGesturePoint(x,y);
	emit updateCanvas(
			QRect(
				QPoint(x-20, y-20),
				QPoint(x+20, y+20)
			)
		);
	//emit DrawingsUpdated();
}
bool Model::saveCEs(QString fileName){
	if(!persistence->saveCEs(fileName, canvasElements))
    return false;
  //if(!persistence->saveModelConnectionsAppend(fileName.toStdString()+str("_aggregations.txt"), ConnectionData::modellIntraconnections))
  //  return false;
  return true;
}
bool Model::loadCEs(QString fileName){
  canvasElements.clear();
  emit removeCanvasElements();
	// load CE-s into a temporary container, than use addCanvasElement add them to canvasElements
	//
	std::list<MyCompositionT<CanvasElement>> tempCanvasElements;
	if(!persistence->loadCEs(this, fileName, tempCanvasElements))
    return false;
	for(auto& e: tempCanvasElements){
		addCanvasElement(std::move(e));//emits canvasElementAdded
	}
	// TODO uncomment if load fails
  //if(!persistence->loadModelConnections(fileName.toStdString()+str("_aggregations.txt"), ConnectionData::modellIntraconnections))
  //  return false;
  
  // Itt azért késlelteve catlakoztatom a Plug-okat, hogy legyen ideje a nézettel kommunikálni, így a CanasEleemnt-ek és 
  // Connectorok geometry-je konzisztens legyen a CEW-ük és ConnWidget-ük geometry-jével
  QTimer::singleShot(1000, [this](){ // if fails probably model got deleted before this could execute. use member func instead of lamda then.
      // reattach plugs to the connector they were attached to before saving and loading
      for(auto& ceIt : canvasElements){
        for(auto& plugIt : ceIt->plugs){
          if(auto plugObs = plugIt.lock()){
            if(auto connectedConnObs = plugObs->getConnectedConn().lock()){
              QPoint connPoint = connectedConnObs->getGeometry().center();
              CE_connectToCEAt(ceIt, connPoint, plugIt);
            }
          }
        }
      }});
  //std::ofstream mlc("mlc.temp"); mlc << "modell layer connections:" << std::endl;
  //saveContainer(mlc, ConnectionData::modellIntraconnections, &ConnectionData::save);
  //std::ofstream eostr("eostr.temp"); eostr  << "existing objects:" << std::endl;
  //for(auto it : Identifiable::existingObjects){
  //  it->save(eostr);
  //  eostr << std::endl;
  //}
  //std::ofstream idsstr("ids.temp"); idsstr  << "ids:" << std::endl;
  //idsstr << Identifiable::ids << std::endl;
  return true;
}
void Model::Widget_saveCEs(QString fileName){
	//saveState();
  if(saveCEs(fileName)){
    emit showMessage("Sikeres mentés");
  }
  else{
    emit showMessage("Hiba mentés közben. nézze meg a log fileokat");
  }
}
void Model::Widget_loadCEs(QString fileName){
  if(loadCEs(fileName)){
    emit showMessage("Sikeres betöltés");
  }
  else{
    emit showMessage("Hiba betöltés közben. nézze meg a log fileokat");
  }
	//saveState();

}
void Model::pushRecoginzeReplaceLastDrawing(){
	if(lastDrawing->getGestures().size() == 0){
		emit showMessage("az ábra üres, ezért nem lehet rögzíteni!");
		return;
	}
	pushLastDrawing();
	assert(getLastDrawing()->isPixelMatrixSetup());
	///tesztNetwork = igaz esetén:
	///betoltunk egy neuralis halot es eldontjuk, hogy melyik ábra-osztályra hasonlít a lastDrawing
	if(testNetwork)
	{
		categorizeDrawingCreateCanvaElement(lastDrawing);
	}
	///newDrawing-ba új Drawing-ot készítünk:
	initNewDrawing();
  // frissítem a Canvas-t, hogy ne az előző Drawing pontjait jelenítse meg
  emit updateCanvas();
}
void Model::widget_enterPressed(){
	pushRecoginzeReplaceLastDrawing();
}
void Model::canvas_mouseEvent(QMouseEvent* event,  std::weak_ptr<CanvasElement> ce, QPoint cCoords){
	//auto element =  srcAsCE();
	cm.applyTransition(
		event,
		Target(cCoords, ce, 1)
	);
	//if(event.type == CanvasEvent::Type::EdgeClicked){
	//	resizizngCEEdge = event.getEdgeClicked();
	//}
	switch(CanvasManipulation::transRes.resultType){
	case Line:{
			addCanvasElement(std::make_shared<LineComponent>(
						this,
						QRect(0,0,0,0), 
						CanvasManipulation::transRes.t1, 
						CanvasManipulation::transRes.t2
						));
			//LineComponent* lcp = lineC.get();
			//std::move(lineC));
		break;
	};
	
	case GestureType:{
			addGestureToActiveDrawing(
						Gesture(
								CanvasManipulation::transRes.t1.getFreshXInCanvas(),
								CanvasManipulation::transRes.t1.getFreshYInCanvas()
						)
					);
		break;
	};
	case GesturePointType:{
		addGesturePoint(
					CanvasManipulation::transRes.t1.getFreshXInCanvas(),
					CanvasManipulation::transRes.t1.getFreshYInCanvas()
				);
		break;
	}
	case NOTHING:{
		break;
	}
	case ELEMENTPRESSED_RESULT:{
		break;
	}
	case ELEMENTSELECTED_RESULT:{
		if(auto so =  selectedCE.lock()){
			so->deselect();
			emit updateCanvas(so->geometry);
		}
		auto newSo = CanvasManipulation::transRes.t1.canvasElement.lock();
		assert(newSo);
		newSo->select();
		selectedCE = newSo;
		emit updateCanvas(newSo->geometry);
		break;
	}
	case ELEMENT_STARTRESIZING_RESULT:
	case ELEMENT_RESIZING_RESULT: {
		auto rt = cm.transRes.t1.canvasElement.lock();
		//ha kijelöltünk valamit, akkor azt fogjuk átméretezni:
		if(auto so = selectedCE.lock()){
			rt = so;
		}
		if(!rt)
			break;		
		emit rt->resizeEdgeSig(
				cm.transRes.t1.getFreshXInCanvas()
				, cm.transRes.t1.getFreshYInCanvas()
				//, resizizngCEEdge
			);
		break;
	}
	case ELEMENT_STOPRESIZING_RESULT: {
		break;
	}
	case ELEMENT_DESELECTED_RESULT:{
		if(auto so =  selectedCE.lock()){
			so->deselect();
			emit updateCanvas(so->geometry);
		}
		break;
	}
	case ELEMENT_DBLPRESSED_RESULT:{
		auto to = CanvasManipulation::transRes.t1.canvasElement.lock();
		QPoint pressPoint = QPoint(
				CanvasManipulation::transRes.t1.x_inCanvasOriginal,
				CanvasManipulation::transRes.t1.y_inCanvasOriginal
		);
		assert(to);
		std::weak_ptr<Connector> pressedConn = to->mouseDoublePressed_My(CanvasManipulation::transRes.t1);
		auto pco = pressedConn.lock();
		if(!pco){
			break;
		}
		
		QRect area = QRect(100,100,200,200);
		auto ctype = *(pco->getConnectableCETypes().begin());
		///TODO: létrehozni kezelni a több fajta CanvasElement-et elfogadó
		///  connecotrokat és itt(is) számítani rájuk. valszeg máshogy kell ezt megoldani mint ahogy elkezdtem; lásd Connector-ban todo comment
		std::weak_ptr<CanvasElement> movedCEO;
		{
		std::shared_ptr<CanvasElement> newCE;
			newCE = CanvasElement::createCanvasElement(this, ctype, area);
		
		/// először newCE-el meg kell hívni addCanvasElement-et, hogy newCE és a nézet réteg közötti connection-ök létrehozza.
		/// addCanvasElement ez törli newCE-t, viszont a visszatér egy weak_ptr-el amit továbbra is tudunk használni:
		movedCEO = addCanvasElement(std::move(newCE));
		/// newCE változó megszűnik, de át move-oltuk már
		}
		to->tryConnToThis(movedCEO, pressPoint, movedCEO.lock()->getMainPlug());
		//connectPlugToConnn(pco, newCE->getMainPlug());
		break;
	}
	case DRAG_UNSELECTED:{
		auto elementObs = ce.lock();
		if(!elementObs){//az esemény entityWidgetből származik-e
			break;
		}
		auto obs = cm.transRes.t1.canvasElement.lock();
		if(obs == nullptr)
			break;
		obs->dragUnselected(
					cm.transRes.t1.x_inCanvasOriginal, 
					cm.transRes.t1.y_inCanvasOriginal
				);
		
		// frissítjük a draggolt entityWidgetet:
		emit obs.get()->should_repaint();
		break;
	}
	case STOPDRAGGING:{
		auto elementObs = ce.lock();
		if(!elementObs){//az esemény entityWidgetből származik-e
			break;
		}
		auto obs = cm.transRes.t1.canvasElement.lock();
		if(obs == nullptr)
			break;
		//auto stoppedAt = getCEAt(event.cPos);
		obs.get()->stopDrag(cCoords);
		
		// frissítjük az összes entityWidgetet:
		for(const auto& it : canvasElements){
			if(it.get()){
				emit it->should_repaint();
			}
		}
		break;		
	}
	default:{
		assert(false);
		break;
	}
	}
	delete event;
}
std::weak_ptr<CanvasElement> Model::addCanvasElement(MyCompositionT<CanvasElement>&& ceb){
	emit canvasElementAdded(ceb->weak_from_this());
  emit ceb->geometryChanged_CEW();
	/// innentől canvasElements menedzseli ceb éllettartamát:
	canvasElements.push_back(std::move(ceb));
  
	return canvasElements.back();
}

//" " közé teszi a stringet,
// hogy egy szóközt tartalmazó elérési útvonal
// system call argumentumként való felhasználásakor ne legyen
// két külön paraméterként értelmezve
std::string Quotes(const std::string& string){
	return str("\"") + string + str("\"");
}
std::string Quotes(const char* cStr){
	return Quotes(std::string(cStr));
}
void Model::trainNNCallback(std::shared_ptr<TrainingUIM> trainingForm){
	if(trainingForm->doPopulateAndConvertBeforeTraining){
		produceDatasFromDir_callback(trainingForm->sourceDir, trainingForm->populatedDir, trainingForm);
	}
	std::vector<std::string> trainingArguments{
		trainingForm->inputSize.toStr()
		, trainingForm->expOutputSize.toStr()
		, trainingForm->epochs_num.toStr()
		, trainingForm->testDataRatio.toStr()
		, trainingForm->batchSize.toStr() 
		, Quotes(trainingForm->concatenatedCsvFileName.toStr())
		, Quotes(trainingForm->trainedNetworkName.toStr())
		, Quotes(trainingForm->ttfModelSaveDir.toStr())
		, trainingForm->ttfModelDoLoadAndRetrain.toStr()
		, Quotes(trainingForm->ttfModelLoadDir.toStr())
	};
	if(persistence->runTrainer(trainingArguments)){
		emit showMessage("tanítás sikeresen megtörtént");
	}
	else{
		emit showMessage("tanítás sikertelen! tekintse meg a log file-okat");
	}
}
void Model::CE_connectToCEAt(std::weak_ptr<CanvasElement> sourceOwner, QPoint pOnCanv, std::weak_ptr<Plug> srcPlug){
	auto srcObserved = sourceOwner.lock();
	assert(srcObserved != nullptr);
  
  /// először lecsatlakoztatom a plugot az aktuális connectoráról:
  if(auto pO = srcPlug.lock()){
    pO->detach();
    /// TODO: itt kéne lecsatlakoztatni a CEW -et is,
    ///  ha azt akarom hogy pontosan akkor legyen csatlakoztatva a CEW mint, amikor a CanvasElement.
    /// most a CEW már a draggolás elején lecsatlakozik, míg a CanvasElement csak a draggolás végén. a LineComponent-re ez nem vonatkozik
  }
  
	for(const auto& ce : canvasElements){
    // dont try to attach the CE to its own connectors
		if(ce.get() && ce.get() != srcObserved.get()){
			std::weak_ptr<Connector> conn = ce.get()->tryConnToThis(srcObserved, pOnCanv, srcPlug);
			if(conn.lock()){
				/// sikerült csatlakozni, nem keresünk Connectorokat tovább
				return;
			}
		}
	}
}

std::vector<std::weak_ptr<CanvasElement>> getNeigh(std::weak_ptr<CanvasElement> curr){
	std::vector<std::weak_ptr<CanvasElement>> ret;
	for(auto& c : curr.lock()->getConnectors()){
		for(auto& p : c.lock()->connectedPlugs){
			if(auto sharedPlug = p.lock()){
				CanvasElement* n = sharedPlug->getContainingCE();
				ret.push_back(n->weak_from_this());
			}
		}
	}
	return ret;
}
template<class T>
struct weakEqual{
	std::weak_ptr<T> t;
	std::owner_less<std::weak_ptr<T>> less;
	weakEqual(std::weak_ptr<T> t_) :t(t_)
	{}
	bool operator()(std::weak_ptr<T> o) const{
		return !less(t,o) && !less(o,t);
	}
};

void h(std::vector<std::weak_ptr<CanvasElement>>& explored, std::vector<std::weak_ptr<CanvasElement>>& exploring, std::weak_ptr<CanvasElement> curr){
	for(auto n : getNeigh(curr)){
		std::weak_ptr<CanvasElement> a;
		std::weak_ptr<CanvasElement> b;
		if(std::find_if(explored.begin(), explored.end(), weakEqual<CanvasElement>(n)) == explored.end()){
			if(std::find_if(exploring.begin(), exploring.end(), weakEqual<CanvasElement>(n)) == exploring.end()){
				exploring.push_back(n);
				h(explored, exploring, n);
			}
		}
	}
	exploring.erase(std::find_if(exploring.begin(), exploring.end(), weakEqual(curr)));
	explored.push_back(curr);
}
std::vector<std::weak_ptr<CanvasElement>> Model::getConnectedRecursive(std::weak_ptr<CanvasElement> s){
	std::vector<std::weak_ptr<CanvasElement>> explored;
	std::vector<std::weak_ptr<CanvasElement>> exploring;
	exploring.push_back(s);
	h(explored, exploring, s);
	return explored;
}
std::list<MyCompositionT<CanvasElement>>::iterator Model::findPosOfCEB(CanvasElement* c){
	assert(c);
	
	int id = c->getId();
	std::list<MyCompositionT<CanvasElement>>::iterator it = canvasElements.begin();
	for( ; it != canvasElements.end() ; ++it){
		if((*it).get() && (*it)->getId() == id){
			break;
		}
	}
	/// ha nem találtuk meg sCEO-t azonosító alapján a canvaselements-ben, akkor hibát dobunk:
	assert(it != canvasElements.end());
	return it;
}
void Model::Wid_copySelected(){
	auto sCEO = selectedCE.lock();
	if(!sCEO){
		return;
	}
  copied = sCEO->weak_from_this();
  copied.lock()->copyFrom(sCEO.get());
}
void Model::Wid_pasteSelected(){
	auto sCEO = selectedCE.lock();
  auto copiedO = copied.lock();
	if(!sCEO || !copiedO){
		return;
	}
  {
  auto copiedToInsert = CanvasElement::createCanvasElement(this, copiedO->CEtype, copiedO->getGeometry());
  copiedToInsert->copyFrom(copiedO.get());
  addCanvasElement(std::move(copiedToInsert));
  }
}
void Model::deleteAt(std::list<MyCompositionT<CanvasElement>>::iterator aCeIterator){
	///kiválasztott elem törlése:
	emit destroyView_W((*aCeIterator)->getId());
	(*aCeIterator).reset();
	canvasElements.erase(aCeIterator);
	// the selected element no longer exists. so i update canvasManipulation
	cm.resetState();
}
void Model::Wid_deleteSelected(){
	if(!selectedCE.lock()){
		return;
	}
	auto connected = getConnectedRecursive(selectedCE);
	for(auto& c : connected){
		assert(c.lock());
		auto found = findPosOfCEB(c.lock().get());
		deleteAt(found);
	}
	selectedCE.reset();
	assert(!selectedCE.lock());
}

void Model::trainNN(){
	std::shared_ptr<UserInputModel> trainingForm(new TrainingUIM("Mintafileok, tanítás"));
	
	emit askForUserInput(trainingForm, [this](std::shared_ptr<UserInputModel> uim){
			auto tUIM = std::dynamic_pointer_cast<TrainingUIM>(uim);
			assert(tUIM);
			this->trainNNCallback(tUIM);
		});
}
void Model::produceDatasFromDir(QString sourceDir, QString destDir){
	std::shared_ptr<UserInputModel> uim(new DrawingPopulationUIM("minták sokszorosítása")); 
	
	emit askForUserInput(uim, [this, sourceDir, destDir](std::shared_ptr<UserInputModel> uim){
		auto casted = std::dynamic_pointer_cast<DrawingPopulationUIM>(uim);
		assert(casted);
		this->produceDatasFromDir_callback(sourceDir, destDir, casted);
	});
}
void Model::produceDatasFromDir_callback(QString sourceDir, QString destDir, std::shared_ptr<DrawingPopulationUIM> uim){
	if(sourceDir == destDir){
		persistence->accessLogger()->warn("Model::produceDatasFromDir_callback(source, dest): source equals dest dir. doing nothing. change dest dir");
		return;
	}
	int categNum = persistence->getDrawingCategoriesNumInDir(sourceDir);
	
	auto u = uim.get();
	///be lehet állitani, hogy a dir-iterátor subdirectory-kba is bele menjen(egy DirIterator flag-el), de ezt most nem használom
	persistence->startIterationInDir(sourceDir, "csv");
	Drawing* d = nullptr;
	std::ofstream populated(u->concatenatedCsvFileName.toStr());
	int categInd = 0;
	std::vector<double> expectedVals(categNum, 0);
	
	while(persistence->nextDrawingCategoryInDir() ){
		if(categInd > 0)
			expectedVals.at(categInd-1) = 0;
		expectedVals.at(categInd) = 1.0;
		std::ifstream is;
		std::ofstream os;
		
		QString destinationFileName = destDir + "/" + persistence->getActualFileName();
		os.open(destinationFileName.toStdString(), std::ios_base::out);
		///elkészítjük a módosított Drawing-okat az aktuális file alapján, és mentjük őket:
		int copyNum = u->populationSize  / std::max(persistence->getDrawingNumInCategory(resHorizontal, resVertical), 1);
		if(copyNum == 0)
			copyNum = 1;
		while(  ( d = persistence->nextDrawingInCategory(resHorizontal, resVertical) )  ){
			for (int i = 0; i < copyNum ; ++i) {
				///a drawing* klónozása:
				Drawing* dCopy = d->copyModifyDrawing(
						u->pointModificationRate, 
						u->minRangeRatio,
						u->maxRangeRatio,
						u->minDeviation,
						u->maxDeviation, 
						u->maxAmplitudeMultiplier, 
						u->minScaleX, 
						u->maxScaleX, 
						u->minScaleY,
						u->maxScaleY, 
						u->scaleChance
				);
				dCopy->print(os, true);
				dCopy->exportPixelMatrixAsLearningData(populated, expectedVals);
				delete dCopy;
			}
			delete d;	
		}
		is.close();
		os.close();
		++categInd;
	}
	emit showMessage("A '" + sourceDir + "' beli adatok sokszorosítása befejeződött");
	populated.close();
}
void Model::categorizeDrawingCreateCanvaElement(Drawing* d){
	//DrawingsInd lastInd = drawings.size()-1;
	if(!testNetwork)
		assert(false);
	auto CEType = dc->classify(d);
  if(CEType == CanvasElement_Type::ERROR){
    QString userMessage = "Az ábra felismerése sikertelen. részletek a log-okban";
    emit showMessage(
      QString(userMessage)
    );
    std::string logWarningMessage = str("Model::categorizeDrawingCreateCanvaElement(): abra felismerese siekertelen");
    persistence->accessLogger()->warn(logWarningMessage);
    return;
  }
  
  
	{
	std::shared_ptr<CanvasElement> uml = nullptr;  
  uml = CanvasElement::createCanvasElement(this, CEType,  d->getBoundingRect());
	if(uml){
		addCanvasElement(std::move(uml));
    if(!CanvasElementTypeNames::CE_map_type_name.count(CEType)){
      qDebug() << "WARNING: didn't find CEType '" << (int)CEType << "' in CanvasElementTypeNames::CE_map_type_name";
      return;
    }
    QString message = tr("substituted canvas element")
                    + ": "
                    + CanvasElementTypeNames::CE_map_type_name.at(CEType);
    emit showMessage(message);
	}
  else{
    emit showMessage(tr("couldn't recognize the drawing"));
  }
	}
}
void Model::pushLastDrawing(){
		lastDrawing->updatePixelMatrix(false);//azért false, mert a Drawing szélsőértékeit minden olyan eljárás frissíti, ami módosítja a drawinghoz gestureoket ad addgesture-rel
		drawings.push_back(lastDrawing);
		int pos = drawings.size() - 1;
		emit addDotMatrixMiniature(drawings[pos]->getPixelMatrix(), pos);
		emit updateCanvas(lastDrawing->getBoundingRect());
}
void Model::discardDrawings(){
	for(uint i = 0 ; i < drawings.size() ; ++i){
		assert(nullptr != drawings[i]);
		auto temp = drawings[i];
		//először null-ra állítom a tömbelemet, és csak aztán szabdítom fell a mutatott memóriaterületet, 
		// hogy a tömböt használó nézetbeli objektumok ne hivatkozhassanak a felszabadított tömbelemre miközben itt felszabadítjuk azokat:
		drawings[i] = nullptr;
		delete temp;
	}
	drawings.resize(0);
	emit removeDrawingDotMatrixMiniatures();
	emit updateCanvas();
}
void Model::discardCanvasElements(){
	for(auto it  = canvasElements.begin() ; it != canvasElements.end() ; ++it){
		if(nullptr == *it)
			continue;
		(*it).reset();
	}
	canvasElements.resize(0);
	emit removeDrawingDotMatrixMiniatures();
	emit removeCanvasElements();
	emit updateCanvas();
}

	
