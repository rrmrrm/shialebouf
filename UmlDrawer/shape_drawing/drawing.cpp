#include "drawing.h"

// seed the random generator at the start of the program
QRandomGenerator Drawing::r(QTime::currentTime().msec());

Drawing::Drawing(){}
Drawing::Drawing(double _standDeviat, int _marginInPixels)//:
	//standDeviat(_standDeviat),
	//gb(_standDeviat),
	//marginInPixels(_marginInPixels)
{
	(void)_standDeviat;
}

Drawing::Drawing(double _standDeviat, int _marginInPixels, double _resolutionHorizontal,  double _resolutionVertical):
	//standDeviat(_standDeviat),
	//gb(_standDeviat),
	resolutionHorizontal(_resolutionHorizontal), 
	resolutionVertical(_resolutionVertical)//,
	//marginInPixels(_marginInPixels)
{
	(void)_standDeviat;
}
Drawing::Drawing(const Drawing& d) :
	gestures(d.gestures),
	pixelMatrix(d.pixelMatrix),
	//standDeviat ( d.standDeviat),
	//gb ( d.gb),
	
	isPixelMatrixSetupBool(d.isPixelMatrixSetupBool),
	resolutionHorizontal(d.resolutionHorizontal),
	resolutionVertical(d.resolutionVertical),

	borderLeft ( d.borderLeft),
	borderTop ( d.borderTop),
	borderRight ( d.borderRight),
	borderBottom ( d.borderBottom),
	maxX(d.maxX),
	maxY(d.maxY),
	minX(d.minX),
	minY(d.minY),
	isBoundsSetup_(d.isBoundsSetup_)
{}
Drawing::~Drawing()
{}
///
/// \brief createDrawingFromFile
/// \param is
/// \param useGestureDataToo
/// \param newResHor
///		ha különbözik a beolvasott resHorizontal értéktől, akkor a gesture-ökből újraelkészítjük a PixelMatrix-t  newResHor szélessgéggel
/// \param newResVert
///		ha különbözik a beolvasott resVertical értéktől, akkor a gesture-ökből újraelkészítjük a PixelMatrix-t  newResVert magassággal
/// \param gestureDataIsInFile
/// \return 
///
Drawing* Drawing::createDrawingFromFile(std::istream& is, int newResHor, int newResVert){
	Drawing* d = new Drawing();
	if(d->load(is, newResHor, newResVert/*, true, true*/)){
		return d;		
	}
	else{
		delete d;
		return nullptr;
	}
  return nullptr;
}
int Drawing::getDrawingLeft() const{
	assert(isBoundsSetup_);
  return getMinX();
	//return marginLeft;
}
int Drawing::getDrawingRight() const{
	assert(isBoundsSetup_);
  return getMaxX();
	//return marginRight;
}
int Drawing::getDrawingTop() const{
	assert(isBoundsSetup_);
  return getMinY();
	//return marginTop;
}
int Drawing::getDrawingBot() const{
	assert(isBoundsSetup_);
  return getMaxY();
	//return marginBottom;
}

const std::vector<std::vector<double>>& Drawing::getPixelMatrix() const{
	return pixelMatrix;
} 
std::vector<std::vector<double>>& Drawing::getPixelMatrix() {
	return pixelMatrix;
} 
QImage& Drawing::accessImage() {
	assert(isPixelMatrixSetup());
	return pixelMatrixImg;
} 
double Drawing::getCellHeight() const{
	//ha nincsenek sorai, akkor is visszakell valamit adni:
	if(getPixelMatrix().size() == 0){
		return 1.0;
	}
	
	return (getDrawingBot() - getDrawingTop()) /(double)(getPixelMatrix()[0].size());
}
QRect Drawing::getBoundingRect() const{
	assert(isBoundsSetup_);
	return QRect(QPoint(getDrawingLeft(), getDrawingTop()),
				 QPoint(getDrawingRight(), getDrawingBot()));
}
uint Drawing::getResolutionVertical() const{
	return resolutionVertical;
}
double& Drawing::getResolutionVertical(){
	return resolutionVertical;
}
uint Drawing::getResolutionHorizontal() const{
	return resolutionHorizontal;
} 
double& Drawing::getResolutionHorizontal(){
	return resolutionHorizontal;
} 

int Drawing::getMinX() const{
	assert(isBoundsSetup_);
	return minX;
}
int Drawing::getMaxX() const{
	assert(isBoundsSetup_);
	return maxX;
}
int Drawing::getMinY() const{
	assert(isBoundsSetup_);
	return minY;
}
int Drawing::getMaxY() const{
	assert(isBoundsSetup_);
	return maxY;
}
bool Drawing::isBoundsSetup() const{
	return isBoundsSetup_;
}
bool& Drawing::isBoundsSetup(){
	return isBoundsSetup_;
}
bool Drawing::isPixelMatrixSetup() const{
	return isPixelMatrixSetupBool;
}
const std::vector<Gesture>&  Drawing::getGestures() const {
	return gestures;
}
std::vector<Gesture>& Drawing::accessGestures() {
	return gestures;
}
void Drawing::addGesture(Gesture g){
	gestures.push_back(g);
	updateBounds(g);
	
}
void Drawing::addGesturePoint(int x, int y){
	// csak akkor szabad új pontot hozzáadni a Drawinghoz, ha már van egy Gesture hozzáadva:
	assert(gestures.size() > 0);
	gestures[gestures.size()-1].addPoint(x,y);
	updateBounds(x,y);
}

// updateBounds_-ot hamisra, akkor nem frissítjük a min,max, bounds  szélsőértékeket.
// viszont ekkor elvaultak lesznek ezek a szélsőértékek mindaddíg, amig meg nem hívjuk az UpdateBounds() eljárást.
// tehát: ha valahonnan updateBounds_ =false-al hívjuk meg ezt az eljárást, akkor majd OTT meg kell hívni az updateBounds()-ot is.
void Drawing::setGesturePointAt(unsigned int ind, int x, int y, bool updateBounds_){
	(void) updateBounds_;
	int flatIndTarget = ind;
	int flatIndCurr = 0;
	uint gInd = 0;
	///invariáns tul: flatIndCurr <= flatIndTarget
	while(gInd < gestures.size() && flatIndCurr <= flatIndTarget){
		///flatIndTarget-flatIndCurr itt pozitív
		uint diff = static_cast<uint>(flatIndTarget-flatIndCurr);
		if(diff < gestures[gInd].getPoints().size()){
			int localInd = flatIndTarget-flatIndCurr;
			/// itt localInd < gest[gInd].size(), ezért gest[gInd].getPoints()[localInd] már a keresett koordinátapárt jelzi.
			gestures[gInd].setPointAt(localInd, x, y);
			updateBounds(x,y);
			return;
		}
		else{
			flatIndCurr += gestures[gInd].getPoints().size();
		}
		++gInd;
	}
	assert(!"ERROR: void Drawing::setGesturePointAt(uint ind, int x, int y): _ind is out of bounds");
}
Drawing* Drawing::clone() const{
	return new Drawing(*this);
}
Drawing* Drawing::copyModifyDrawing(double pointModificationRate, double minRangeRatio,  double maxRangeRatio, double minDeviation, double maxDeviation, double maxAmplitudeMultiplier, double minScaleX, double maxScaleX, double minScaleY, double maxScaleY, double scaleChance ) const{
	//assert(drawing != nullptr);
	Drawing* d = clone();
	double boundingSize = std::max(getMaxX() - getMinX(), getMaxY() - getMinY());
	///görbe módosítások:
	for(int i = 0 ; i < (int)d->getGesturePointsNum() ; ++i){
		if( r.generateDouble() < pointModificationRate){
			double angle = r.generateDouble() * 2.0 * 3.14159265;
			int range = (
							minRangeRatio 
							+ r.generateDouble() * (maxRangeRatio - minRangeRatio)
						) 
						* d->getGesturePointsNum() / 2.0;
			///a (1.0-r.generateDouble()) kifejezés miatt a standDeviation (0,maxStandDeviation] intervalumban lesz,
			/// ez a zért kell, mert standDeviationnel osztani fog a program a GaussBlur1D-nek az f(..) függvényében.
			double standDeviation = minDeviation + (1.0-r.generateDouble()) * (maxDeviation-minDeviation);
			double amplitudeMult= r.generateDouble() * maxAmplitudeMultiplier * boundingSize;
			GaussBlur1D gB(standDeviation, amplitudeMult);
			for (int j = std::max(0, i-range) ; j < std::min(i+range, (int)d->getGesturePointsNum()-1) ; ++j) {
				double dist = gB.f(j-i);
				int xOriginal = d->gesturePointAt(j).first;
				int yOriginal = d->gesturePointAt(j).second;
				d->setGesturePointAt(j, xOriginal + qCos(angle) * dist, yOriginal + qSin(angle) * dist, false);
			}
		}
	}
	/// frissiteni kell a drawing szélső értékeit mielott tovabb modositanank, mert setGesturePointAt(...,..., false) elavulttá tezi a minmax, bounds,  szélsőértékeket, 
	d->updatePixelMatrix(true);
	///skálázás:
	
	if(r.generateDouble() < scaleChance ){
		double scaleX = minScaleX + r.generateDouble()*(maxScaleX-minScaleX);
		double scaleY = minScaleY + r.generateDouble()*(maxScaleY-minScaleY); 
		int leftBorder = d->getMinX();
		int upperBorder = d->getMinY();
		for (uint i = 0; i < d->getGesturePointsNum(); ++i) {
			int pX = d->gesturePointAt(i).first;
			int pY = d->gesturePointAt(i).second;
			int dX = pX - leftBorder;
			int dY = pY - upperBorder;
			d->setGesturePointAt(i, leftBorder + dX * scaleX, upperBorder + dY * scaleY);
		}
	}
	
	/// itt is frissiteni kell a drawing szélső értékeit
	/// azért is mert a cellák se azt ábrázolnják már, ami a gesture-ökben van, 
	///  ezert szólni kell a drawing-nak, hogy újra készítse ell a cellatáblát - vagyis az ábrát - a gestureinek a pontjaiból:
	d->updatePixelMatrix(true);
	return d;
}

void Drawing::updateBounds(){
	if(gestures.size() == 0)
		return;
	minX = std::numeric_limits<int>().max();
	int cmp;
	for (uint i = 0; i < gestures.size(); ++i) {
		if(gestures[i].getMinX(&cmp) && cmp < minX){
			minX = cmp;
			isBoundsSetup_ = true;
		}
	}
	maxX = std::numeric_limits<int>().lowest();
	for (uint i = 0; i < gestures.size(); ++i) {
		if(gestures[i].getMaxX(&cmp) && cmp > maxX){
			maxX = cmp;
			isBoundsSetup_ = true;
		}
	}
	minY = std::numeric_limits<int>().max();
	for (uint i = 0; i < gestures.size(); ++i) {
		if(gestures[i].getMinY(&cmp) && cmp < minY){
			minY = cmp;
			isBoundsSetup_ = true;
		}
	}
	maxY = std::numeric_limits<int>().lowest();
	for (uint i = 0; i < gestures.size(); ++i) {
		if(gestures[i].getMaxY(&cmp) && cmp > maxY){
			maxY = cmp;
			isBoundsSetup_ = true;
		}
	}
	// ha semelyik gesture-ben se voltak beállítva a minmax X és Y értékek,
	//  akkor nincs értelme a többi korlát-értéket beállítani:
	if(!isBoundsSetup_){
		return;
	}

	borderLeft = minX;
	borderTop = minY;
	
	int innerWidth = maxX - borderLeft;
	int innerHeight = maxY - borderTop; 
	
	borderRight = borderLeft + innerWidth;
	borderBottom = borderTop + innerHeight;
	assert(maxX >= minX);
	assert(maxY >= minY);
}
void Drawing::updateBounds(int newX, int newY){
	if(isBoundsSetup_){
		maxX = maxX<newX ? newX : maxX;
		maxY = maxY<newY ? newY : maxY;
		minX = minX>newX ? newX : minX;
		minY = minY>newY ? newY : minY;
	}
	else{
		maxX = newX;
		maxY = newY;
		minX = newX;
		minY = newY;
	}
	borderLeft = minX;
	borderTop = minY;
	
	int innerWidth = maxX - borderLeft;
	int innerHeight = maxY - borderTop; 
	
	borderRight = borderLeft + innerWidth;
	borderBottom = borderTop + innerHeight;
	assert(maxX >= minX);
	assert(maxY >= minY);
	isBoundsSetup_ = true;
}
void Drawing::updateBounds(const Gesture& newG){
	int gMinX;
	int gMinY;
	int gMaxX;
	int gMaxY;
	//ha az ujGesture-ben nincs beállítva a min és max X és Y értékek, akkor nem kell semmit változtatni:
	if(!newG.getMaxX(&gMaxX) 
			|| !newG.getMaxY(&gMaxY) 
			|| !newG.getMinX(&gMinX) 
			|| !newG.getMinY(&gMinY) ){
		return;
	}
	// ha nincsenek beállítva a Drawing határai és szélsőértékei,
	//  akkor az új Gestrue-től egyszerűen átvesszük azokat:
	if(!isBoundsSetup_){
		maxX = gMaxX;
		maxY = gMaxY;
		minX = gMinX;
		minY = gMinY;
	}
	else{
		//amúgy össze kell hasonlítani az eddigi szálsőértékeket az újakkal:
		maxX = getMaxX() < gMaxX ? gMaxX : maxX;
		maxY = getMaxY() < gMaxY ? gMaxY : maxY;
		minX = getMinX() > gMinX ? gMinX : minX;
		minY = getMinY() > gMinY ? gMinY : minY;
	}
	borderLeft = minX;
	borderTop = minY;
	
	int innerWidth = maxX - borderLeft;
	int innerHeight = maxY - borderTop; 
	
	borderRight = borderLeft + innerWidth;
	borderBottom = borderTop + innerHeight;
	assert(maxX >= minX);
	assert(maxY >= minY);
	isBoundsSetup_ = true;
}

void Drawing::clampCellValues(){
	for (uint col = 0; col < resolutionHorizontal; ++col) {
		for (uint row = 0; row < resolutionVertical; ++row) {
			if(pixelMatrix[col][row] > 1.0){
				pixelMatrix[col][row] = 1.0;
			}
		}
	}
}
unsigned int Drawing::getGesturePointsNum() const{
	int s = 0;
	for (int i = 0; i < (int)gestures.size(); ++i) {
		s += gestures[i].getPoints().size();
	}
	return  s;
}
const std::pair<int,int>& Drawing::gesturePointAt(unsigned int ind ) const{
	int flatIndTarget = ind;
	int flatIndCurr = 0;
	uint gInd = 0;
	///invariáns tul: flatIndCurr <= flatIndTarget
	while(gInd < gestures.size() && flatIndCurr <= flatIndTarget){
		///flatIndTarget-flatIndCurr itt pozitív
		uint diff = static_cast<uint>(flatIndTarget-flatIndCurr);
		if( diff < gestures[gInd].getPoints().size()){
			int localInd = flatIndTarget-flatIndCurr;
			/// itt localInd < gest[gInd].size(), ezért gest[gInd].getPoints()[localInd] már a keresett koordinátapárt jelzi.
			return gestures[gInd].getPoints()[localInd];
		}
		else{
			flatIndCurr += gestures[gInd].getPoints().size();
		}
		++gInd;
	}
	assert(!"ERROR: _ind is out of bounds: std::pair<int,int>& Drawing::getPoint(uint _ind)");
	return gestures[gInd].getPoints()[0];//csak hogy a fordító ne problémázzon a 'return' mentes lefutási ág miatt
}

void Drawing::updatePixelMatrix(bool updateBoundsToo){
	if(updateBoundsToo)
		updateBounds();
	//gb.standardDeviaton = standDeviat;
	
	isPixelMatrixSetupBool = true;
	assert(0 != resolutionHorizontal);
	assert(0 != resolutionVertical);
	/// készítünk egy QPixmap-ot:
	
	//pixelMatrixPm = QPixmap(resolutionHorizontal,resolutionVertical);
	pixelMatrixPm = QPixmap(	getDrawingRight()-getDrawingLeft()+1,
						 getDrawingBot()-getDrawingTop()+1);
	{
	QPainter p(&pixelMatrixPm);
	/// a pixelMatrixPm pixmapot kitöltjük fekete színnel:
	p.fillRect(
			0, 
			0, 
			getDrawingRight()-getDrawingLeft()+1,
			getDrawingBot()-getDrawingTop()+1, 
			QBrush(QColor(0,0,0))
	   );
	pixelMatrix.resize(resolutionHorizontal);
	/// gesture-öket rárajzoljuk a pixelMatrixPm pixmap-re:
	p.setPen(QPen(QColor(255,255,255)));
	for (int i = 0; i < (int)getGestures().size(); ++i) {
		for (int j = 0; j < (int)getGestures()[i].getPoints().size()-1; ++j) {
			int x0 = getGestures()[i].getPoints()[j].first - getDrawingLeft();
			int y0 = getGestures()[i].getPoints()[j].second - getDrawingTop();
			int x1 = getGestures()[i].getPoints()[j+1].first - getDrawingLeft();
			int y1 = getGestures()[i].getPoints()[j+1].second - getDrawingTop();
			p.drawLine(x0,y0,x1,y1);
		}
	}
	}
	///többlépésben átméretezzük pixelMatrixPm-et:
	
	///reshor, vert = 30, 30 esetén nekünk 30x30as kép kell
	/// de a skálázott kép(scaledPm) lehet mondjuk 30x20-as lesz(úgy skálázzuk ugyanis, hogy megtartsa a méretarányokat)
	
	QPixmap scaledPm = pixelMatrixPm.scaled(resolutionHorizontal, resolutionVertical, Qt::KeepAspectRatio, Qt::SmoothTransformation);
	//scaledPm.save("_pixelMatrixPm_scaled.png","PNG",100);
	
	pixelMatrixPm = scaledPm;
	/// ezért csinálunk egy új 30x30-as pixmapet (newPm-et), és rárajzoljuk scaledPm-et:
	QPixmap newPixmap(resolutionHorizontal, resolutionVertical);
	QPainter p2(&newPixmap);
	p2.fillRect( QRect(0,0,resolutionHorizontal, resolutionVertical), QBrush(QColor(0,0,0)) );
	p2.drawPixmap(0,0, scaledPm);
	
	/// az elkészült newPixmap-ból feltöltjük a pixelMatrix táblázatot:
	pixelMatrixImg = newPixmap.toImage();
	for (uint col = 0; col < pixelMatrix.size(); ++col) {
		pixelMatrix[col].resize(resolutionVertical);
		for (uint row = 0; row < pixelMatrix[col].size(); ++row) {
			pixelMatrix[col][row] = qGray(pixelMatrixImg.pixel(col,row))/255.0;
		}
	}
	clampCellValues();
	
}
std::ostream& Drawing::print(std::ostream& os, bool printGestureDataToo) const{
	os << 0 << std::endl;
  //os << marginInPixels << std::endl;
  os << 0 << std::endl;// for backward compatibility with old saved drawings(they had an integer field named marginInPixels)
	os.precision(std::streamsize(5));
	write2DTable(os, pixelMatrix);
	os << isBoundsSetup_ << std::endl;
	os << isPixelMatrixSetup() << std::endl;
	os << resolutionHorizontal << '\t' << '\t' << resolutionVertical << std:: endl;
	if (printGestureDataToo) {
		os << gestures;
	}
	os << std::endl;
	return os;
}
std::ostream& Drawing::exportPixelMatrixAsLearningData(std::ostream& os, std::vector<double>& expectedVals) const{
	bool first = true;
	for(int i = 0 ; i < resolutionVertical ; ++i){
		for(int j = 0 ; j < resolutionHorizontal ; ++j){
			if(first){
				first = false;
			}
			else{
				os << ',';
			}
			os << pixelMatrix.at(j).at(i);
		}		
	}
	for(double d : expectedVals){
		os << ',' << d;
	}
	os << std::endl;
	return os;
}

std::istream& Drawing::load(std::istream& is, int newResHor, int newResVert/*, bool useGestureDataToo, bool gestureDataIsInFile*/){

	bool isPixmapSetup = false;
	double unused;
	is >> unused;
	//gb = GaussBlur2D(standDeviat);
  //is >> marginInPixels;
  int comp; is >> comp;// for backward compatibility with old drawing savefiles(they have saved with them, a marginInpixels integer field)
	//is >> unused;
	read2DTable(is, pixelMatrix);
	is >> isBoundsSetup_;
	is >> isPixelMatrixSetupBool;
	is >> resolutionHorizontal >> resolutionVertical;

	if(!is.fail() /*&& gestureDataIsInFile*/){
		if(is.peek() == EOF){
			assert(!"ERROR: Drawing::load(..): gestureDataIsInFile is set to true, but end of stream is reached before Gesture could be read!");
		}
		
		if(true){
			is >> gestures;
			///gesture-ből beállítom a Drawing paramétereit:
			updateBounds();
			///ha át kell méretezni a PixelMatrix táblát, akkor azt itt teszem meg:
			if(resolutionVertical != newResVert || resolutionHorizontal != newResHor){
				resolutionVertical = newResVert;
				resolutionHorizontal = newResHor;
				updatePixelMatrix(false);
				isPixmapSetup = true;
			}
		}

		if(is.fail()){
			assert(!"ERROR: Drawing::load(..): error during reading gesture data!");
		}
	}
	///ha valaminek a beolvasása nem sikerült, akkor itt nem próbáljuk meg használni a hibásan beállított értékeket:
	if(!is)
		return is;
	
	if(!isPixmapSetup){
		pixelMatrixPm = QPixmap(resolutionHorizontal, resolutionVertical);
		QPainter painter(&pixelMatrixPm);
		
		painter.fillRect( QRect(0, 0, resolutionHorizontal, resolutionVertical), 
						  QBrush(QColor(0,0,0)) 
						);
		/// pixelMatrixImg megfelelő méretre állítása:
		pixelMatrixImg = pixelMatrixPm.toImage();
		//QPainter p(&pixelMatrixPm);
		//pixelMatrix.resize(resolutionHorizontal);
		
		/// pixelMatrixImage beállítása a fent elkészített pixelMatrix-ből:
		for (uint col = 0; col < pixelMatrix.size(); ++col) {
			//pixelMatrix[col].resize(resolutionVertical);
			for (uint row = 0; row < pixelMatrix[col].size(); ++row) {
				/// a [0,1] interval beli double -> [0,255] interval beli int konverzió során kerekítési hiba lép fel.
				/// 0.5 hozzáadaásával a kerekítési hibakorlát 0.5 lesz
				
				/// ha sokszor konvertáljuk át más méretűre ugyanazt az ábrát,
				///  akkor ez a konverzió sokszor történik meg, így a hiba nagyobb is lehet.
				int l = 255.0 * pixelMatrix[col][row] + 0.5;
				pixelMatrixImg.setPixel(col, row, qRgb(l,l,l));
				/// forDebug változóba visszakonvertálom a most beállított pixelt 0..255 közötti értékre QRgb-ből, hogy ellenőrízzem,
				///  hogy qGray(qRgb(l,l,l)) == l:
				double forDebug = qGray(pixelMatrixImg.pixel(col,row));
				if(std::abs(forDebug - l) > 0.5){
					assert(false);
				}
			}
		}
		/// pixelMatrixPm beállítása pixelMatrixImg-ből:
		QPixmap pm2(resolutionHorizontal, resolutionVertical);
		QPainter painter2(&pm2);
		painter2.fillRect( QRect(0, 0, resolutionHorizontal, resolutionVertical), 
						  QBrush(QColor(0,0,0)) 
						);
		pm2.fromImage(pixelMatrixImg);//todo vmiert fekete teljesen a kep 
		//pm2.save("_loaded_pixelMatrixPm.png","PNG",100);
		painter.drawPixmap(0,0, pm2);
	}
	return is;
}
