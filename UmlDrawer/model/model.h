#ifndef MODEL_H
#define MODEL_H

#include "persistence/persistence.h"
#include "idrawingcontainer.h"
#include "userinputmodel.h"
#include "common.hpp"
#include "shape_drawing/drawing.h"
#include "identityDir/identifiable.h"
#include "canvasmanipulation.h"
#include "DrawingClassifiers/neuralclassifier.h"
#include "shape_drawing/drawing.h"
#include "shape_drawing/drawing.h"
#include "model/CanvasElements/canvaselement.h"
#include "compositiont.h"
#include <QTimer>
#include <QObject>

#include <map>
#include <list>
#include <functional>
class Connector;
class CanvasEvent;

#ifdef TESTING 
#define  privatet public
#define  protectedt public
#else
#define  privatet private
#define  protectedt protected
#endif
class Model : 
		public QObject,  
		public virtual Identifiable, 
		public virtual IDrawingContainer
{
	Q_OBJECT
	friend class Persistence;
	typedef CanvasManipulation::State State;
	
///fields:
privatet:
	int saveStateInd = 0;
	CanvasManipulation cm;
	
	Persistence* persistence;
	std::weak_ptr<CanvasElement> selectedCE;
	//Direction resizizngCEEdge;
	std::shared_ptr<GenericFileUIM> saveLastDrawingModel;
	
	uint resHorizontal;
	uint resVertical;
	bool testNetwork = false;
	std::string drawingsOutFileName;
	
	int marginInPixels;
	double drawingBlurSD;
	// ///
	// /// \brief isDragging
	// /// a felhasználó épp húzza-e draggol-e épp az egérrel
	// /// (azaz billentyűt lenyomva húzza-e épp az egeret)
	// bool isDragging = false;
	// CanvasElement* lastDraggedCE = nullptr;
	//std::shared_ptr<CanvasElement> copied;
  AggregationT<CanvasElement> copied;
	IDrawingClassifier* dc = nullptr;
public:
	/// canvasElements heterogén kollekció a rajzvásznon elhelyezkedő nézet-elemek modelljének(CanvasElement-ek) tárolására.
	/// 
	/// Megjegyzés: Azért shared_ptr eket tárolok ptr-eke helyett a canvasElements -ben, hogy át tudjam adni az elemeit más osztályoknak anélkül,
	///  hogy a vektor reallokálná az átadott pointer által mutatott memória területet
	/// (reallokáció történhet amikor új elemet adunk a vektorhoz)
	/// a canvasElements elemeit weak_ptr formájában adom át.
	/// 
	/// Megjegyzés2: Azt is akarom, hogy a model bármikor törölhesse az elemeket anélkül, hogy ez problémát okozna a nézet rétegben.
	/// 
	/// Megjegyzés3: alternatív megoldás lehet std::list -ben tárolni az elemeket, és a nézetben ezen elemekre pointereket átadni.
	/// Így nincs reallokáció új elem hozzáadásakor. 
	/// Ezt nem próbáltam még ki, de lehet a baj:
	///  míg a weak_ptr lock() eljárása biztosítja hogy ne szabaduljon fel a nézetben épp használatban levő változó(persze csak a lock() hívás és a scope végének elérése között),
	/// egy sima pointer használatával ez nincs biztosítva. Tehát amennyiben a modell felszabadítja a tárolt canvasElementet amit a nézet épp használ egy eljárása közepén, akkor a program segfaultot dobhat.
	/// Ezen alternatíva használatához biztosítani kell még azt, hogy a modell nem töröl canvasElements -beli elemeket amikor a nézet egy eljárása épp használja azt.
	/// Lehet, hogy a qt signal slot hívása ezt eleve biztosítják és nem lesz ezzel probléma.
	std::list<MyCompositionT<CanvasElement>> canvasElements;
	Drawing* lastDrawing;
	std::vector<Drawing*> drawings;
public:
	Model();
	~Model();
	
privatet:
	void displayDrawingsAsMiniatures();
  void initNewDrawing();
  void pushRecoginzeReplaceLastDrawing();
  void categorizeDrawingCreateCanvaElement(Drawing*);
public:
	std::weak_ptr<GenericFileUIM> getSaveLastDrawingModel();
	const std::vector<Drawing*>&  getDrawings() const override;
	const Drawing* getLastDrawing() const;
	bool addGestureToActiveDrawing(Gesture g);
	void addGesturePoint(int x, int y);
	
	bool saveCEs(QString fileName);
	bool loadCEs(QString fileName);
	void Widget_saveCEs(QString fileName);
	void Widget_loadCEs(QString fileName);
	void widget_enterPressed();
	void canvas_mouseEvent(QMouseEvent*, std::weak_ptr<CanvasElement>, QPoint cCoords);
	///
	/// \brief addCanvasElement
	/// jelet küld a nézetnek, hogy az hozza létre a szükséges connection-öket a nézet és ceb között.
	/// ezután move-olja ceb-et a canvasElements-be. 
	/// z eljárás visszatér egy weak_ptr-el, amin keresztül továbbra is elérhető a CanvasElement.
	/// \param ceb	/
	/// \return 
	/// a canvasElements-be move-olt ceb weak_ptr-ként
	///	
	std::weak_ptr<CanvasElement> addCanvasElement(MyCompositionT<CanvasElement>&& ceb);
	void trainNNCallback(std::shared_ptr<TrainingUIM> trainingForm);
	
	std::vector<std::weak_ptr<CanvasElement>> getConnectedRecursive(std::weak_ptr<CanvasElement> s);
	std::list<MyCompositionT<CanvasElement>>::iterator findPosOfCEB(CanvasElement* c);
	void Wid_copySelected();
	void Wid_pasteSelected();
	void deleteAt(std::list<MyCompositionT<CanvasElement>>::iterator);
	void Wid_deleteSelected();
	
public slots:
	void CE_connectToCEAt(std::weak_ptr<CanvasElement> src, QPoint p, std::weak_ptr<Plug> srcPlug);
	void trainNN();
	///
	///\brief produceDatasFromDir
	///
	///2 mappaválasztó dialog ugrik fel, amiben egy forrás és egy célkönyvtárat kell kiválasztani.
	///a forráskönyvtárban levő összes filet Drawingként értelmezve
	///ezek Gesture-adatai alapján több Drawing file-t készít a célkönyvtárban úgy, hogy a Gesture-ök pontjait kicsit eltolja véletlenszerűen
	///FONTOS:
	/// 1.: a forráskönyvtárban csak Drawing fileoknak szabad lennie,
	/// 2.: a Drawing file-ok Gesture datát is kell, hogy tartalmazzanak
	///megjegyzés: az  elkészített képekre az e-programban  használt margó és elmosás adatok vonatkoznak majd,
	///  tehát így ezeket a paramétereket is lehet módosítani
	void produceDatasFromDir(QString sourceDir, QString destDir);
	void produceDatasFromDir_callback(QString sourceDir, QString destDir, std::shared_ptr<DrawingPopulationUIM>);	
	
	///a pushAndTestLastDrawing használom, ahol a 'lastDrawing'-ot a 'drawings' vektorba helyezése után,
	/// de a lecserélése előtt még fel akarom használni a lastDrawing-ot:
	void pushLastDrawing();
	void discardDrawings();
	void discardCanvasElements();
signals:
	void removeCanvasElement(std::shared_ptr<CanvasElement> ceb);
	void updateCanvas(QRect);
	void updateCanvas();
	void askUserForFileName(std::function<void (QString path)> callback);
	void askForUserInput(std::shared_ptr<UserInputModel>, std::function<void (std::shared_ptr<UserInputModel>)> callback);
	void showMessage(QString);
	void addDotMatrixMiniature(DotMatrix, int pos_canBeOverIndexed);
	void removeDotMatrixMiniature(int pos);
	void removeDrawingDotMatrixMiniatures();
	void canvasElementAdded(std::weak_ptr<CanvasElement> ce);
	void destroyView_W(IdType);
	void removeCanvasElements();
};

#endif // MODEL_H
