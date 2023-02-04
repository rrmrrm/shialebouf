#ifndef DRAWING_HPP
#define DRAWING_HPP

#include "functions.hpp"
#include "shape_drawing/gesture.h"
#include "common.hpp"
#include "persistence/streamoperators.h"

#include <QObject>
#include <QTime>
#include <QPixmap>
#include <QImage>
#include <QPainter>
#include <QRgb>
#include <QString>//uint
///QRandomGenerator pótlása, amennyiben régi verziójú a qt:
#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0) //5.10-nek felel meg ez remélhetőleg
    #include <QRandomGenerator>
#else
    #include <qrandomgenerator.h>
#endif

#include <iostream>
#include <vector>
#include <limits.h>
extern bool readAndSaveGestureData;
class Model;
class Drawing{
	friend Model;
protected:
	//bool visible = true;
	std::vector<Gesture> gestures;
	///indexing of pixelMatrix: pixelMatrix[pixCol][pixRow]
	///az ábra képpont-cellái. a programban a képernyő pixeleiből ezen cellákba konvertáljuk az ábrákat,
	/// ennek megfelelően a továbbiakban: a pixel a képernyő egy képpontját, a cella pedig a már átkonvertált ábra egy pontját jelenti .
	std::vector< std::vector<double> > pixelMatrix;
	QPixmap pixelMatrixPm;
	QImage pixelMatrixImg;
	//double standDeviat = 1;
	
	//jelzi, hogy meghivtak-e mar ay update() fuggvenyt,
	//tehat a cellak be lettek-e allitva:
	bool isPixelMatrixSetupBool = false;
	/// number of cell columns in the Drawing:
	double resolutionHorizontal = 0;
	/// number of pixelMatrix in each column in the Drawing:
	double resolutionVertical = 0;
	//int marginInPixels = 0;
	//// margókat már nem használom
	//int marginLeft = 0;
	//int marginTop = 0;
	//int marginRight = 0;
	//int marginBottom = 0;
	int borderLeft = 0;
	int borderTop = 0;
	int borderRight = 0;
	int borderBottom = 0;
	int maxX = 0;
	int maxY = 0;
	int minX = 0;
	int minY = 0;
	bool isBoundsSetup_ = false;
	
	// /// \brief drawPointAdditive
	// /// increases (color-)value of a cell
	// /// \param cellX
	// /// \param cellY
	// /// \param value
	// ///
	// void drawPointAdditive(uint cellX, uint cellY, double value);
	static QRandomGenerator r;
public:
	Drawing();
	Drawing(double _standDeviat, int _marginInPixels);
	
	Drawing(double _standDeviat, int _marginInPixels, double _resolutionHorizontal,  double _resolutionVertical);
	Drawing(const Drawing& d);
	virtual ~Drawing();
	///
	/// \brief createDrawingFromFile
	/// \param is
	/// \param newResHor
	///		ha különbözik a beolvasott resHorizontal értéktől, akkor a gesture-ökből újraelkészítjük a PixelMatrix-t  newResHor szélessgéggel
	/// \param newResVert
	///		ha különbözik a beolvasott resVertical értéktől, akkor a gesture-ökből újraelkészítjük a PixelMatrix-t  newResVert magassággal
	/// \return 
	///
	static Drawing* createDrawingFromFile(std::istream& is, int newResHor, int newResVert);
	//TODO these 4 are not needed anymore, use min,-max getters
  virtual int getDrawingLeft() const;
	virtual int getDrawingRight() const;
	virtual int getDrawingTop() const;
	virtual int getDrawingBot() const;
	
	//virtual double getCellWidth() const;
	const std::vector<std::vector<double>>& getPixelMatrix() const;
	std::vector<std::vector<double>>& getPixelMatrix() ;
	QImage& accessImage() ;
	virtual double getCellHeight() const;
	QRect getBoundingRect() const;
	uint getResolutionVertical() const;
	double& getResolutionVertical();
	uint getResolutionHorizontal() const;
	double& getResolutionHorizontal();
	
	int getMinX() const;
	int getMaxX() const;
	int getMinY() const;
	int getMaxY() const;
	int getMarginInPixels() const;
	int& getMarginInPixels();
	// void setVisible(bool v);
	// bool isVisible() const;
	bool isBoundsSetup() const;
	bool& isBoundsSetup();
	bool isPixelMatrixSetup() const;
	//bool& isPixelMatrixSetup();
	const std::vector<Gesture>&  getGestures() const ;
	std::vector<Gesture>&  accessGestures() ;
	void addGesture(Gesture g);
	void addGesturePoint(int x, int y);
	
	// updateBounds_-ot hamisra, akkor nem frissítjük a min,max bounds  szélsőértékeket.
	// viszont ekkor elvaultak lesznek ezek a szélsőértékek mindaddíg, amig meg nem hívjuk az UpdateBounds() eljárást.
	// tehát: ha valahonnan updateBounds_ =false-al hívjuk meg ezt az eljárást, akkor majd OTT meg kell hívni az updateBounds()-ot is.
	void setGesturePointAt(unsigned int ind, int x, int y, bool updateBounds_ = true);

	virtual Drawing* clone() const;
	///
	/// \brief CopyModifyDrawing
	///a Canvas projectben a Widget-beli produceDatasFromDir függvény használja,
	/// hogy egy mappában levő drawing fileokból kicsit módosított és több Drawing-ot tartalmazó drawing datafileokat csináljon
	///a bool useGestureDataToo és bool gestureDataIsInFile paramétereket azért nem kell megadni ennek a metódusnak,
	/// mert feltételezzük, hogy van Gesture data a fileban,
	/// ugyanis a Gesture pontok alapján készítjük el a módosított Drawing-ot.
	/// \param drawing
	/// ezt a drawing-ot másolja le, és módosítja a metódus. nem lehet nullptr
	/// \param pointModificationRate
	/// a Gesture pontjai ilyen relatív eséllyel lesznek módosítva(0- egyik pont se, 1- az összes pont módosul)
	/// \param range
	/// ekkora sugarú méretű környezetben tolódhatnak el a módosított pontok
	/// 
	/// ugy hogy egyenkent a drawing gestur-einek minden pontja körül 'pointModificationRate' esellyel 
	/// történik modositas(nevezzünk egy módosításra kijelölt pontot "csúcsnak"),
	///  és a körülötte 'maxRangeRatio' sugaron belül lévő pontokkal együtt 
	/// normaleloszlas függvény 
	/// szerint veletlen(de az aktuális csúcsra tekintve állandó) iranyba eltolja a pontokat.
	/// Minden pontot sorraveszünk, és a a pontok 'pointModificationRate' eséllyel lesznek "csúcs"-nak választva.
	/// Ezután függőleges és vízszintes irányba skálázzuk a képet véletlenszerű mértékben.
	Drawing* copyModifyDrawing(double pointModificationRate, double minRangeRatio,  double maxRangeRatio, double minDeviation, double maxDeviation, double maxAmplitudeMultiplier, double minScaleX, double maxScaleX, double minScaleY, double maxScaleY, double scaleChance ) const;

protected:
	
	///a gestures alapján beállítja a gestures-re vonatkozó szélsőértékeket(border..., margin...)
	void updateBounds();
	void updateBounds(int newX, int newY);
	void updateBounds(const Gesture& newG);
	void clampCellValues();
public:
	
	
	unsigned int getGesturePointsNum() const;
	//void drawPoint(uint cellCol, uint cellRow);
	const std::pair<int,int>& gesturePointAt(unsigned int ind ) const;

	virtual void updatePixelMatrix(bool updateBoundsToo = false);
	virtual std::ostream& print(std::ostream& os, bool printGestureDataToo) const;
	virtual std::ostream& exportPixelMatrixAsLearningData(std::ostream& os, std::vector<double>& expectedVals) const;
	
	///TODO: alaposabb kiiras a fileban: tobb adat kiirasa, és a beolvasott sorok elé kiírni, hogy mely változóhoz tartoznak, ezek alapján elleőrizni, hogy jó adatokat olvasunk-e be
	/// (pl azert, mert most a gestureDataIsInFile és useGestureDataToo változókról nem lehet tudni, hogy a fileban igazak, vagy hamisak
	/// (ha nem kompatibilis filebol olvasunk, azt a program nem észleli, eért ilyen esetben kiszámíthatatlan a működés)	
	///
	/// \brief Drawing::load
	/// \param is
	/// \param newResHor
	///		ha különbözik a beolvasott resHorizontal értéktől, akkor a gesture-ökből újraelkészítjük a PixelMatrix-t  newResHor szélessgéggel
	/// \param newResVert
	///		ha különbözik a beolvasott resVertical értéktől, akkor a gesture-ökből újraelkészítjük a PixelMatrix-t  newResVert magassággal
	/// \param readGestureDataToo
	/// if set to true, then we try to read gesture data too from the file
	/// if readGestureDataToo is set to true, BUT no gesture data is present in the file,
	///  the program fails on an assert
	/// \return 
	///
	virtual std::istream& load(std::istream& is, int newResHor, int newResVert/*, bool useGestureDataToo, bool gestureDataIsInFile*/);
};

#endif // DRAWING_HPP
