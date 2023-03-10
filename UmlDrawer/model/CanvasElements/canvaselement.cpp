#include "model/CanvasElements/canvaselement.h"
#include "model/myqline.h"
#include "model/model.h"
#include "persistence/streamoperators.h"
#include "model/plug.h"
#include "model/plug.h"

#include <list>
#include <iterator>
#include <QTimer>

template<class Iter, class Container>
Iter myPrev(Iter it, Container& c, bool& succ){
	Iter ret = it;
	if(ret == c.begin()){
		succ = false;
		return ret;
	}
  if(ret != c.begin() && ret != ++c.begin() && ret != ++ (++c.begin()) && ret != c.end()){
    qDebug();
  }
	--ret;
	succ = true;
	return ret;
}
template<class Iter, class Container>
Iter myNext(Iter it, Container& c, bool& succ){
	Iter it2 = it;
	if(it2 == c.end()){
		succ = false;
		return c.end();
	}
	++it2;
	if(it2 == c.end()){
		succ = false;
		return c.end();
	}
	succ = true;
	return it2;
}

LineComponent::LineComponent(Model* model_, QRect geometry_)
	:CanvasElement(model_, CanvasElement_Type::LINET, geometry_),
		pl1(std::make_shared<Plug>(this)),
		pl2(std::make_shared<Plug>(this)),
		connRel1(std::make_shared<Connector>(this, CanvasElementTypeGroups::RELATIONSHIPS)),
		connRel2(std::make_shared<Connector>(this, CanvasElementTypeGroups::RELATIONSHIPS)),
		connTB1(std::make_shared<Connector>(this, CanvasElement_Type::TEXTBOXT)),
		connTB2(std::make_shared<Connector>(this, CanvasElement_Type::TEXTBOXT))
	  
{
	plugs.push_back(pl1);
	plugs.push_back(pl2);
	connectors.push_back(connRel1);
	connectors.push_back(connRel2);
	connectors.push_back(connTB1);
	connectors.push_back(connTB2);
	
	//forwardPlugSig(pl1.get());
	//forwardPlugSig(pl2.get());
	//arrangeConns();
}
LineComponent::LineComponent(
		Model* model_, 
		QRect geometry_, 
		Target start_, 
		Target end_
	) 
	:
		CanvasElement(model_, CanvasElement_Type::LINET, geometry_),
		start(start_),
		end(end_),
		pl1(std::make_shared<Plug>(this)),
		pl2(std::make_shared<Plug>(this)),
		connRel1(std::make_shared<Connector>(this, CanvasElementTypeGroups::RELATIONSHIPS)),
		connRel2(std::make_shared<Connector>(this, CanvasElementTypeGroups::RELATIONSHIPS)),
		connTB1(std::make_shared<Connector>(this, CanvasElement_Type::TEXTBOXT)),
		connTB2(std::make_shared<Connector>(this, CanvasElement_Type::TEXTBOXT))
{
	plugs.push_back(pl1);
	plugs.push_back(pl2);
	connectors.push_back(connRel1);
	connectors.push_back(connRel2);
	connectors.push_back(connTB1);
	connectors.push_back(connTB2);
	draggingLineIt = lineSegments.end();
	
	//forwardPlugSig(pl1.get());
	//forwardPlugSig(pl2.get());
	QTimer::singleShot( 1000, this, &LineComponent::connectToInitialConnectors);
}
LineComponent::~LineComponent(){
	
}
void LineComponent::updateConnectorDirs(){
  if(lineSegments.empty())
    return;
  auto l1 = lineSegments.front();
  auto l2 = lineSegments.back();
  connRel1->setDir(l1.getOtherDir());
  connRel2->setDir(l2.getDir());
}
void LineComponent::addLineSegment(MyQLine&& l){
	lineSegments.push_back(std::move(l));
  updateConnectorDirs();
}
void LineComponent::addLineSegmentToFront(MyQLine&& l){
	lineSegments.push_front(std::move(l));
  updateConnectorDirs();
}

std::ostream& LineComponent::save(std::ostream& os){
	CanvasElement::save(os);
	
	os << lineSegments ;
	//saveIter(os, lineSegments, draggingLineIt);
	
	os << draggedLinePart << std::endl;
	os << margin << std::endl;
	start.save(os);
	end.save(os);
	saveSharedPtr(os, pl1);
	saveSharedPtr(os, pl2);
	saveSharedPtr(os, connRel1);
	saveSharedPtr(os, connRel2);
	saveSharedPtr(os, connTB1);
	saveSharedPtr(os, connTB2);
	return os;
}
std::istream& LineComponent::load(std::istream& is){
	CanvasElement::load(is);
	
	is >> lineSegments;
  draggingLineIt = lineSegments.end();
	//loadIter(is, lineSegments, draggingLineIt);
	int d; is >> d; draggedLinePart = (LinePart)d;
	is >> margin;
	start.load(is);
	end.load(is);
	loadSharedPtr(is, pl1, this);
	loadSharedPtr(is, pl2, this);
	loadSharedPtr(is, connRel1, this, CanvasElementTypeGroups::RELATIONSHIPS);
	loadSharedPtr(is, connRel2, this, CanvasElementTypeGroups::RELATIONSHIPS);
	loadSharedPtr(is, connTB1, this, CanvasElement_Type::TEXTBOXT);
	loadSharedPtr(is, connTB2, this, CanvasElement_Type::TEXTBOXT);
	return is;
}

std::ostream& LineComponent::saveAggregations(std::ostream& os){
	CanvasElement::saveAggregations(os);
	start.saveAggregations(os);
	end.saveAggregations(os);
	pl1->saveAggregations(os);
	pl2->saveAggregations(os);
	connRel1->saveAggregations(os);
	connRel2->saveAggregations(os);
	connTB1->saveAggregations(os);
	connTB2->saveAggregations(os);
	return os;
}
std::istream& LineComponent::loadAggregations(std::istream& is){
	CanvasElement::loadAggregations(is);
	start.loadAggregations(is);
	end.loadAggregations(is);
	pl1->loadAggregations(is);
	pl2->loadAggregations(is);
	connRel1->loadAggregations(is);
	connRel2->loadAggregations(is);
	connTB1->loadAggregations(is);
	connTB2->loadAggregations(is);
	return is;
}

std::weak_ptr<Plug> LineComponent::getMainPlug() const{
	return pl1;
}
bool LineComponent::isInDirection(QPoint p0, Direction dir, QPoint p1, bool strict) const{
	switch (dir) {
	case LEFT:{
		return p1.x() < p0.x() || (!strict && p1.x() == p0.x());
		break;
	}
	case RIGHT:{
		return p1.x() > p0.x() || (!strict && p1.x() == p0.x());
		break;
	}
	case TOP:{
		return p1.y() < p0.y() || (!strict && p1.y() == p0.y());
		break;
	}
	case BOT:{
		return p1.y() > p0.y() || (!strict && p1.y() == p0.y());
		break;
	}
	}
	return false;
}

//returns whether d is in exclusive range (r0, r1):
bool inRangeExcl(int r0, int r1, int d){
	return d > r0 && d < r1;
}
bool LineComponent::isWidgetInWay(const CanvasElement* ceb,
				   Direction dir,
				   QPoint p,
				   QPoint bound, 
				   int margin,//a widgetet ennzivel sz??lesebbnek tekintj??k az ??tk??z??s detekt??l??sakor
				   QPoint* OUTcollosionPoint)
				{
	bool ret = false;
	QPoint collosionPoint;
  int ceLeft = ceb->x();
  int ceRight = ceb->x() + ceb->width();
  int ceTop = ceb->y();
  int ceBot = ceb->y() + ceb->height();
	switch (dir) {
	case LEFT:{
		collosionPoint = QPoint(ceRight + margin, p.y());
		ret = inRangeExcl(ceTop-margin, ceBot + margin, p.y())
					&& ceRight + margin < p.x()
					&& bound.x() < ceRight + margin;
		break;
	}
	case RIGHT:{
		collosionPoint = QPoint(ceLeft - margin, p.y());
		ret = inRangeExcl(ceTop-margin, ceBot + margin, p.y())
					&& ceLeft - margin > p.x()
					&& bound.x() > ceLeft - margin;
		break;
	}
	case TOP:{
		collosionPoint = QPoint(p.x(), ceBot + margin);
		ret = inRangeExcl(ceLeft-margin, ceRight + margin, p.x())
					&& ceBot + margin < p.y()
					&& bound.y() < ceBot + margin;
		break;
	}
	case BOT:{
		collosionPoint = QPoint(p.x(), ceTop - margin);
		ret = inRangeExcl(ceLeft-margin, ceRight + margin, p.x())
					&& ceTop - margin > p.y()
					&& bound.y() > ceTop - margin;
		break;
	}
	}
	if(ret){
		if(OUTcollosionPoint != nullptr)
			*OUTcollosionPoint = collosionPoint;
	}
	return ret;
}

void LineComponent::extendInDirIf(QPoint& lineEndToUpdate, Direction dir, QPoint extensionBound){
	if(!isInDirection(lineEndToUpdate, dir, extensionBound, false)){
		return;
	}
	MyQLine newLine;
	switch (dir){
	case LEFT:{
	}
	case RIGHT:{
		//pEnd = QPoint(extensionBound.x(), lineEndToUpdate.y());
		newLine.fromBounds(lineEndToUpdate, extensionBound, HORIZONTAL);
		break;
	}
	case TOP:{
	}
	case BOT:{
		//pEnd = QPoint(lineEndToUpdate.x(), extensionBound.y());
		newLine.fromBounds(lineEndToUpdate, extensionBound, VERTICAL);
		break;
	}
	}
	lineEndToUpdate = newLine.p2();
	addLineSegment(std::move(newLine));
	//lineSegments.push_back(std::move(newLine));
}
void LineComponent::rotate90ToPoint(Direction& line1Dir, QPoint p0, QPoint p1){
	switch (line1Dir) {
	case LEFT:{
	}
	case RIGHT:{
		if(p1.y() > p0.y())
			line1Dir = BOT;
		else
			line1Dir = TOP;
		break;
	}
	case TOP:{
	}
	case BOT:{
		if(p1.x() > p0.x())
			line1Dir = RIGHT;
		else
			line1Dir = LEFT;
		break;
	}
	}
}
QPoint LineComponent::getWidgetAttPoint(Direction dir, const CanvasElement* ceb, int margin) const{
	switch (dir) {
	case LEFT:{
		return QPoint(ceb->x() - margin, 0);
		break;
	}
	case RIGHT:{
		return QPoint(ceb->x() + ceb->width() + margin, 0);
		break;
	}
	case TOP:{
		return QPoint(0, ceb->y() - margin);
		break;
	}
	case BOT:{
		return QPoint(0, ceb->y() + ceb->height() + margin);
		break;
	}
	}
	return QPoint();
}
QPoint LineComponent::getWidgetAttPoint(const Target& target) const{
	auto ceObs = target.canvasElement.lock();
	assert(ceObs != nullptr);
	//az target koordin??tarendszer??ben meghat??rozzuk a k??z??ppontj??t??l val??
	// v??zszintes ??s f??gg??leges eltol??st:
	int wMidX = ceObs.get()->width()/2;
	int wMidY = ceObs.get()->height()/2;
	int horizontalDisplacement = target.x_inTarget - wMidX;
	int vertDisplacement = target.y_inTarget - wMidY;
	
	QPoint ret;
	//ha a v??zszintes ir??nyban van legt??volabb a widget k??z??ppontj??t??l a target:
	if(std::abs(horizontalDisplacement) >= std::abs(vertDisplacement)){
		if(horizontalDisplacement >= 0)
			ret = QPoint(ceObs.get()->geometry.right(), target.getFreshYInCanvas());
		else 
			ret = QPoint(ceObs.get()->geometry.left(), target.getFreshYInCanvas());
	}
	else{// ha f??gg??leges ir??nyban van legt??volabba k??z??ppontt??l:
		if(vertDisplacement >= 0)
			ret = QPoint(target.getFreshXInCanvas(), ceObs.get()->geometry.bottom());
		else
			ret = QPoint(target.getFreshXInCanvas(), ceObs.get()->geometry.top());
	}
	return ret;
}
QPoint LineComponent::adjustCoordInDimension(QPoint p, Dimension dir, QPoint newCoord){
	switch (dir) {
	case HORIZONTAL:{
		return QPoint(newCoord.x(), p.y());
		break;
	}
	case VERTICAL:{
		return QPoint(p.x(), newCoord.y());
		break;
	}
	}
	return QPoint(newCoord.x(), p.y());
}
void LineComponent::attachToElement(const std::list<MyQLine>::iterator& l, LinePart part, std::weak_ptr<Connector> conn, const Target& t){
	assert(l == lineSegments.begin() || l == std::prev(lineSegments.end()));
	auto connO = conn.lock();
	assert(connO);
	auto ceObs = t.canvasElement.lock();
	if(!ceObs)
		return;
	//csak akkor ny??jtjuk a vonalat a canvasElelmt oldal??hoz, ha j?? ir??nyban illeszkedik hozz??:
	Direction attachedSide = connO->getDir();
	QPoint attPoint = getWidgetAttPoint(t);
	if(part == START){
		l->movePartAlongLine(attPoint, part);
		/// ha rossz ir??ny?? a vonal utols?? szegmense, akkor megtoldjuk egy ??jal:
		if(dirToDim(attachedSide) != dirToDim(l->getDir())){
			addLineSegmentToFront( MyQLine(attPoint, l->p1(), dirToDim(attachedSide)) );
			//lineSegments.push_front(MyQLine(attPoint, l->p1(), dirToDim(attachedSide)));
		}
	}
	if(part == END){
		l->movePartAlongLine(attPoint, part);
		if( dirToDim(attachedSide) != dirToDim(l->getOtherDir()) ){
			addLineSegment(
							   MyQLine(l->p2(), attPoint, dirToDim(attachedSide))
							   );
		}
	}
}
void LineComponent::moveLines(const std::list<MyQLine>::iterator& lineIt, const QPoint& newPlace, LinePart lp){
	if(lineSegments.end() == lineIt){
		qDebug() << "LineComponent::moveLines INVALID LINE: lineIt==lineSegments.end()" << '\n';
		return;
	}
	switch(lp){
	case MIDDLE:{
		break;
	}
	case START:{
		bool previousExists = false;
		bool prevpreviousExists = false;
		std::list<MyQLine>::iterator prev = myPrev(lineIt, lineSegments, previousExists);
		if(previousExists){
			prev->movePartAlongLine(newPlace, END);
			prev->moveTransversal(newPlace);
		}
		std::list<MyQLine>::iterator prevprev = myPrev(prev, lineSegments, prevpreviousExists);
		if(prevpreviousExists){
			prevprev->movePartAlongLine(newPlace, END);
		}
		
		bool nextExists = false;
		lineIt->moveTransversal(newPlace);
		lineIt->movePartAlongLine(newPlace, START);
		
		std::list<MyQLine>::iterator next = myNext(lineIt, lineSegments, nextExists);
		if(nextExists){
			next->movePartAlongLine(newPlace, START);
		}
		break;
	}
	case END:{
		bool previousExists = false;
		std::list<MyQLine>::iterator prev = myPrev(lineIt, lineSegments, previousExists);
		if(previousExists){
			prev->movePartAlongLine(newPlace, END);
		}
		
		bool nextExists = false;
		bool nextnextExists = false;
		lineIt->moveTransversal(newPlace);
		lineIt->movePartAlongLine(newPlace, END);
		std::list<MyQLine>::iterator next = myNext(lineIt, lineSegments, nextExists);
		if(nextExists){
			next->moveTransversal(newPlace);
			next->movePartAlongLine(newPlace, START);
		}
		std::list<MyQLine>::iterator nextnext = myNext(next, lineSegments, nextnextExists);
		if(nextnextExists){
			nextnext->movePartAlongLine(newPlace, START);
		}
		break;
	}
	}
	//arrangeConns();
}

void LineComponent::reconnect(){
	if(lineSegments.empty())
		return;
	if(auto c1O = pl1->getConnectedConn().lock()){
		QPoint p1(start.getFreshXInCanvas(),start.getFreshYInCanvas());
		moveLines(lineSegments.begin(), p1, START);
		attachToElement(lineSegments.begin(), START, c1O, start);
	}
	if(auto c2O = pl2->getConnectedConn().lock()){
		QPoint p2(end.getFreshXInCanvas(),end.getFreshYInCanvas());
		moveLines(std::prev(lineSegments.end()), p2, END);
		attachToElement(std::prev(lineSegments.end()), END, c2O, end);
	}
	emit linesChanged_CEW();
}

void LineComponent::recreateLineSegments(){
	auto ce1 = start.canvasElement.lock();
	auto ce2 = end.canvasElement.lock();
	auto linesPreState = lineSegments;
	if( ce1 == nullptr || ce2 == nullptr){
		addLineSegment(
			MyQLine(
				HORIZONTAL, 
				start.x_inCanvasOriginal, 
				end.x_inCanvasOriginal,
				start.y_inCanvasOriginal
			)
		);
		addLineSegment(
			MyQLine(
				VERTICAL, 
				start.y_inCanvasOriginal, 
				end.y_inCanvasOriginal,
				end.x_inCanvasOriginal
			)
		);
	}
	else{
		/// line1Dir, line2Dir:
		/// 1. ??s 2. vonalkezdem??ny n??z??si ir??nya.
		/// line1Dir ir??nyba pr??b??ljuk meg megtoldani az 1. szegment??ltvonal v??g??t,
		/// ha ezzel k??zelebb jutunk a 2 vinalstegmens ??sszek??t??s??hez:
		
		/// meghat??rozom a vonal ??ltal ??sszekapcsolt k??t CE ??sszecsatolt Connector-j??t:
		auto c1O = pl1->getConnectedConn().lock();
		auto c2O = pl2->getConnectedConn().lock();
		/// meghat??rozom, hogy a t??rtvonal els?? ??s utols?? szegmense milyen ir??nyba csatlakozzon a megfelel?? Connector-okhoz:
		Direction line1Dir;
		Direction line2Dir;
		if(c1O){
			line1Dir = c1O->getDir();
		}
		else{
			///ha pl1 egy connector-hoz se csatlakozik, akkor RIGHT ir??ny?? lesz az els??(??s utols??) vonalszegmens:
			line1Dir = RIGHT;
		}
		if(c2O){
			line2Dir = c2O->getDir();
		}
		else{
			///ha pl2 nincs egy connector-hoz se csatlakozik, akkor RIGHT ir??ny?? lesz az utols??(??s els??) vonalszegmens:
			line2Dir = RIGHT;
			
		}
		
		lineSegments.clear();
		// a 2 EntityWidget vonalhoz csatlakoz?? oldalainak meghat??roz??sa, ??s
		//  vonalkezdem??nyek ki??p??t??se a 2 widgetb??l(el??re defini??lni ezen vonalkezdem??nyek hossz??t. lehet mondjuk 30 pixel)
		
		// az els?? vonalkezdem??nyt fogjuk ??jabb szegmensekkel kieg??sz??teni, hogy ??ssze??rjen a 2. vonalkezdem??nnyel
		
		// vonalkezdem??nynek t??bb szegmense is lehet, ??s a vonalkezdem??ny ir??nya 
		// ??ltal??ban a benne lev?? utols?? vonalszegmens ir??ny??t jelenti, de vannak esetek, amikor ett??l elt??r.
		// sz??val pontosabban a vonalkezdem??ny "n??z??s??nek" az ir??ny??t jelenti.
		// N??z??si ir??ny alatt azt az ir??nyt ??rtem, amelyik ir??nyba pr??b??ljuk meghosszab??tani a vonalkezdem??nyt,
		//  hogy ??sszek??thess??k a 2. vonalkedem??nnyel(??s ha nem ez a j?? ir??ny, akkor megv??ltoztatjuk a n??z??si ir??nyt)
		
		int minimalHangoutDist = 40;
		int margin = 10;
		int ce1Left = ce1->x();
		int ce1Right = ce1->x() + ce1->width();
		int ce1Top = ce1->y();
		int ce1Bot = ce1->y() + ce1->height();
		
		int ce2Left = ce2->x();
		int ce2Right = ce2->x() + ce2->width();
		int ce2Top = ce2->y();
		int ce2Bot = ce2->y() + ce2->height();
		QPoint line1Start;
		QPoint line1End;
		switch (line1Dir) {
		case LEFT:{
			line1Start = QPoint( ce1Left, start.getFreshYInCanvas());
			line1End = QPoint( line1Start.x() - minimalHangoutDist, line1Start.y());
			break;
		}
		case RIGHT:{
			line1Start = QPoint( ce1Right, start.getFreshYInCanvas());
			line1End = QPoint( line1Start.x() + minimalHangoutDist, line1Start.y());
			break;
		}
		case TOP:{
			line1Start = QPoint( start.getFreshXInCanvas(), ce1Top);
			line1End = QPoint( line1Start.x(), line1Start.y() - minimalHangoutDist);
			break;
		}
		case BOT:{
			line1Start = QPoint( start.getFreshXInCanvas(), ce1Bot);
			line1End = QPoint( line1Start.x(), line1Start.y() + minimalHangoutDist);
			break;
		}
		}
		//az els?? vonalkezdem??ny els?? szegmens??t elt??roljuk a lineSegments t??mbben:
		addLineSegment(
		   MyQLine(
			   line1Start,
			   line1End,
			   dirToDim(line1Dir)
		));
		QPoint line2Start;
		QPoint line2End;
		switch (line2Dir) {
		case LEFT:{
			line2Start = QPoint( ce2Left, end.getFreshYInCanvas());
			line2End = QPoint( line2Start.x() - minimalHangoutDist, line2Start.y());
			break;
		}
		case RIGHT:{
			line2Start = QPoint( ce2Right, end.getFreshYInCanvas());
			line2End = QPoint( line2Start.x() + minimalHangoutDist, line2Start.y());
			break;
		}
		case TOP:{
			line2Start = QPoint( end.getFreshXInCanvas(), ce2Top);
			line2End = QPoint( line2Start.x(), line2Start.y() - minimalHangoutDist);
			break;
		}
		case BOT:{
			line2Start = QPoint( end.getFreshXInCanvas(), ce2Bot);
			line2End = QPoint( line2Start.x(), line2Start.y() + minimalHangoutDist);
			break;
		}
		}
		//(a 2. vonalkezdem??nyt a k??t vonal ??ssze??r??sekor mentj??k a lineSegments-be)
		
		// ISM??TELNI am??g a k??t vonalkezdem??ny ??ssze nem ??r, de maximum mondjuk 6 iter??ci??ig:
		//	 HA
		//	  1.: az 1. vonalkezdem??ny - line1Dir ir??nyban val?? -meghosszabb??t??s??val az 1. vonalkezdem??ny ir??ny??ban a 2 vonalkezdm??ny t??vols??ga T??NYLEGESEN cs??kken ??S
		//	  2.: nincs ??tban se az 1. , se a 2. widget
		//	 AKKOR, 
		//			az 1. vonalkezdem??ny meghosszabb??t??sa line1Dir ir??nyban, hogy pont egy vonalban legyen a 2. vonalkezdem??nnyel
		//	 AM??GY:
		//			HA 1. nem teljes??l(t??vols??g nem cs??kkenne), akkor 
		//				meghat??rozni, hogy a vonalkezdem??nyre mer????leges 2 ir??ny k??z??l merre van a 2. vonalkezdem??ny v??ge, ??s
		//				ebben az ir??nyba "fordulni"(AKKOR IS FORDULNI KELL, ha egyik ir??ny sem jobb a m??sikn??l) 
		//				(lehet, hogy ebben az ir??nyban ??tk??zn??nk a widgettel, ez??rt nem h??zhatunk vonalat, de meg kell v??ltoztatni az ir??nyt, hogy ezesetben a widgettel val?? ??tk??z??st ??szlelje a program, ??s megker??lje a widgetet a k??vetkez?? iter??ci??kban.)
		//			AM??GY HA 2. nem teljes??l(??tban van az 1, , vagy 2. widget), akkor 
		//				az 1. vonalkezdem??nyt meghosszab??tani, hogy neki??rjen az ??tk??z?? widgetnek,  
		//				 azt??n mer??leges ir??nyban meghosszab??tani ??gy, hogy a hosszab??t??s ir??ny??ban t??ll??gjon (MENNYIVEL?) a widgeten, ??s
		//				 v??lasszuk a lehets??ges k??z??l azt, amelyik ir??nyba vana 2. vonalkezd. v??ge(AKKOR IS V??LASSZUNK ir??nyt, ha mindk??t ir??ny egyar??nt j??)
		//	ha el??rt??k az iter??ci??limitet, akkor felhaszn??l?? fel?? jelezni, hogy sikertelen volt a vonal l??trehoz??sa
		//  egyir??ny?? egym??smelletti vonalszegmensek ??sszevon??sa
		//  az ellent??tes ir??ny?? egym??smelletti szegmenseket sz??ttolni egym??st??l, azt??n mer??leges szegmenssel ??sszek??tni ??ket
		//    a sz??ls?? szegmenseket nem szabad mozgatni. ha az egyik szegmens sz??ls?? szegmens, akkor a m??sikat kell eltolni.
		uint iter = 0;
		uint allowedIterations = 20;
		bool linesConnected = false;
		while( !linesConnected && iter < allowedIterations){
			bool inDir = isInDirection(line1End, line1Dir, line2End, true);
			QPoint collosionPoint;
			bool ce1Blocking = isWidgetInWay(ce1.get(), line1Dir, line1End, line2End, margin, &collosionPoint);
			bool ce2Blocking = isWidgetInWay(ce2.get(), line1Dir, line1End, line2End, margin, &collosionPoint);
			bool clearPath = !ce1Blocking && !ce2Blocking;
			if(inDir && clearPath){
				extendInDirIf(line1End, line1Dir, line2End);
			}
			else{
				if(!inDir){
					//(!inDir && !clearPath) || (!inDir && clearPath)
					rotate90ToPoint(line1Dir, line1End, line2End);
				}
				else{
					//(inDir && !clearPath)
					extendInDirIf(line1End, line1Dir, collosionPoint);
					rotate90ToPoint(line1Dir, line1End, line2End);
					if(ce1Blocking)
						extendInDirIf(line1End, line1Dir, getWidgetAttPoint(line1Dir, ce1.get(), margin));
					else
						extendInDirIf(line1End, line1Dir, getWidgetAttPoint(line1Dir, ce2.get(), margin));
				}
			}
			linesConnected = (line1End == line2End);
			++iter;
		}
		if(linesConnected){
			//2. vonalkezdem??ny egyetlen szegmens??t hozz??adjuk a vonalszegmensekhez:
			//a 2. vonal a 2. widgett??l indul, ez??rt kell megcser??lni a kezd?? ??s v??gpontj??t, amikor az els?? vonalhoz csatolom
			addLineSegment(
							   MyQLine(line2End, line2Start, dirToDim(line2Dir))
							   );
			//lineSegments.push_back(MyQLine(line2End, line2Start, dirToDim(line2Dir)));
		}
		else{
			lineSegments.resize(0);
		}
		// egyir??ny?? egym??smelletti vonalszegmensek ??sszevon??sa:
		mergeLinesWhenNeeded();
	}
	//auto prevDraggingLineIt = draggingLineIt;
	//arrangeConns();
	draggingLineIt = lineSegments.end();
	// itt resetelem a draggingLineIt iter??tort, mert invalid'l=dott a vonalsyegmensek ujra??p??t??sekor

	emit linesChanged_CEW();
}

void LineComponent::mergeLinesWhenNeeded(){
	int mergeTreshold = 5;
	if(lineSegments.begin() != lineSegments.end() && std::next(lineSegments.begin()) != lineSegments.end()){
		auto l2 = std::next(lineSegments.begin());
		for( ; l2 != lineSegments.end() ; ++l2){
			auto l1 = std::prev(l2);
			if(l1->getDim() == l2->getDim()){
				l2->movePartAlongLine(l1->p1(),START, false);
				l1 = lineSegments.erase(l1);
			}
		}
	}
	for(auto l = lineSegments.begin() ; l != lineSegments.end() ; ++l){
		
		if(std::abs(l->dx()) <= mergeTreshold && std::abs(l->dy()) <= mergeTreshold){
			QPoint erasedLineEnd = l->getPart(END);
			l = lineSegments.erase(l);// megjegyz??s: itt tov??bbiter??ljuk l-t
			//itt lehet, hogy el??rt??k a lineSegments v??g??t ??s t??liter??ltunk az erase-el, 
			// ha ez t??rt??nt, akkor a ciklusb??l kiugrunk
			if(l == lineSegments.end())
				break;
			l->moveTransversal(erasedLineEnd, false);
			bool nextExists;
			auto next = myNext(l, lineSegments, nextExists);
			if(nextExists){
				next->movePartAlongLine(l->getPart(END), START, false);
			}
			
			bool previousExists;
			l = myPrev(l, lineSegments, previousExists);
		}
	}
	// m??gegyszer ??szevonom az egym??smelletti p??rhuzamos vonalakat, 
	// mert az el??z?? ciklusban keletkezhetett p??r egym??smelletti p??rhuzamos vonal:
	if(lineSegments.begin() != lineSegments.end() && std::next(lineSegments.begin()) != lineSegments.end()){
		auto l2 = std::next(lineSegments.begin());
		for( ; l2 != lineSegments.end() ; ++l2){
			auto l1 = std::prev(l2);
			if(l1->getDim() == l2->getDim()){
				l2->movePartAlongLine(l1->p1(),START, false);
				l1 = lineSegments.erase(l1);
			}
		}
	}
	for(const MyQLine& it : lineSegments){
		it.validate();
	}
  // az els?? vagy utols?? vonalszegmens m??r lehet, m??s ir??ny?? mint az eddigi,
  //  ez??rt friss??tem a rajtuk lev?? connectorok ir??ny??t
  updateConnectorDirs();
	//arrangeConns();
	emit linesChanged_CEW();
}
void LineComponent::dragUnselectedOverrideThis(int canvas_x, int canvas_y){
	QPoint asPoint(canvas_x, canvas_y);
	if(!isDragging){
		auto linesPreState = lineSegments;
		auto prevDraggingLineIt = draggingLineIt;
		
		//0 ??s 1 k??z??tti val??s sz??m. ha vonal elejre kattintotak akkor 0, ha v??g??re akkor 1 lesz:
		double linedivisionRatio = 0;
		draggingLineIt = getLineItAtCanvasCoords(canvas_x, canvas_y, &linedivisionRatio);

		if(lineSegments.end() != draggingLineIt){
			if(linedivisionRatio <= 0.5){
				draggedLinePart = START;
				bool previousExists = false;
				myPrev(draggingLineIt, lineSegments, previousExists);
				if(!previousExists){
					//ha ez az els?? szegmens:
					//draggingLineIt->moveTransversal(asPoint);
					draggingLineIt->movePartAlongLine(asPoint, START);
					
					MyQLine newLine = MyQLine(draggingLineIt->p1(), asPoint, draggingLineIt->getOtherDim());
					addLineSegmentToFront(std::move(newLine));
					//lineSegments.push_front(std::move(newLine));
					
					//ezut??n m??r az ??jonna l??trehozott vonalat akarjuk draggolni.
					//draggedLinePart vonatkozik az ??j draggoland?? elemre is(az uj vonal ugyanazon oldal??t kell draggolni, mint az eredetinek):
					--draggingLineIt;
				}
			}
			else{
				draggedLinePart = END;
				bool nextExists = false;
				myNext(draggingLineIt, lineSegments, nextExists);
				if(!nextExists){//ha ez az utols?? szegmens:
					draggingLineIt->movePartAlongLine(asPoint, END);
					MyQLine newLine = MyQLine(draggingLineIt->p2(), asPoint, draggingLineIt->getOtherDim());
					addLineSegment(std::move(newLine));
					//lineSegments.push_back(std::move(newLine));
					
					//ezut??n m??r az ??jonna l??trehozott vonalat akarjuk draggolni.
					//draggedLinePart vonatkozik az ??j draggoland?? elemre is(az uj vonal ugyanazon oldal??t kell draggolni, mint az eredetinek):
					++draggingLineIt;
				}
			}
		}
	}
	else{
		if(lineSegments.end() != draggingLineIt){
			moveLines(draggingLineIt, asPoint, draggedLinePart);
			/// start ??s end a t??rtvonal ??j v??gei alapj??n friss??l:
			if(auto sO = start.canvasElement.lock()){
				start.x_inTarget = lineSegments.front().x1() - sO->x();
				start.y_inTarget = lineSegments.front().y1() - sO->y();
			}
			if(auto eO = end.canvasElement.lock()){
				end.x_inTarget = lineSegments.back().x1() - eO->x();
				end.y_inTarget = lineSegments.back().y1() - eO->y();
			}
		}
		else{
		}
		
	}
	emit linesChanged_CEW();
	should_repaint();
}
void LineComponent::dragSelected(int canvas_x, int canvas_y){
	(void) canvas_x;
	(void) canvas_y;
	
}
void LineComponent::stopDragOverrideThis(QPoint cPos){	
	auto linesPreState = lineSegments;
	
	if(draggingLineIt != lineSegments.end()){
		bool previousExists = false;
		bool nextExists = false;
		
		myPrev(draggingLineIt, lineSegments, previousExists);
		myNext(draggingLineIt, lineSegments, nextExists);
		// ha draggingLineIt az els?? vonalszegmens, ??s a v??g??t draggoljuk, akkor az ??j ponton lev?? widget-hez pr??b??lunk csatlakozni:
		if(!previousExists && draggedLinePart == START){
      model->CE_connectToCEAt(weak_from_this(), cPos, pl1);
		}
		// ha draggingLineIt az utols?? vonalszegmens, ??s az elej??t draggoljuk, akkor az ??j ponton lev?? widget-hez pr??b??lunk csatlakozni:
		if(!nextExists && draggedLinePart == END){
      model->CE_connectToCEAt(weak_from_this(), cPos, pl2);
		}
	}
	mergeLinesWhenNeeded();
	draggingLineIt = lineSegments.end();
}
QRect createRect(Direction dir, QPoint pDirSideMid, int lLongitudinal, int lLAteral){
	int w = -1, h= -1;
	switch(dir){
	case RIGHT:{
		w=lLongitudinal; h= lLAteral; 
		return QRect(pDirSideMid-QPoint(w,h/2), pDirSideMid+QPoint(0,h/2)); break;
	}
	case LEFT:{
		w=lLongitudinal; h= lLAteral; 
		return QRect(pDirSideMid-QPoint(0,h/2), pDirSideMid+QPoint(w,h/2)); break;
	}
	case BOT:{
		w=lLAteral; h= lLongitudinal; 
		return QRect(pDirSideMid-QPoint(w/2,h), pDirSideMid+QPoint(w/2,0)); break;
	}
	case TOP:{
		w=lLAteral; h= lLongitudinal; 
		return QRect(pDirSideMid-QPoint(w/2,0), pDirSideMid+QPoint(w/2,h)); break;
	}
	}
	return QRect();
}

std::list<MyQLine>::iterator LineComponent::getLineItAtCanvasCoords(int cx, int cy, double* OUTlinedivisionRatio){
	std::list<MyQLine>::iterator  l0 = lineSegments.begin();
	std::list<MyQLine>::iterator l = l0;
	int margin = 12;
	double ratio = 0;
	bool p = false;
	while( !p && l != lineSegments.end()){
		switch(l->getDir()){
		case LEFT:{
			if(l->x1() - l->x2() != 0)
				ratio = (double)(l->x1() - cx) / (double)(l->x1() - l->x2());
			p = QRect(l->p2()-QPoint(margin,margin), l->p1()+QPoint(margin,margin))
						.contains(QPoint(cx, cy));
			break;
		}
		case TOP:{
			if(l->y1() - l->y2() != 0)
				ratio = (double)(l->y1() - cy) / (double)(l->y1() - l->y2());
			p = QRect(l->p2()-QPoint(margin,margin), l->p1()+QPoint(margin,margin))
						.contains(QPoint(cx, cy));
			break;
		}
		case RIGHT:{
			if(l->x2() - l->x1() != 0)
				ratio = (double)(cx - l->x1()) / (double)(l->x2() - l->x1());
			p = QRect(l->p1()-QPoint(margin,margin), l->p2()+QPoint(margin,margin))
						.contains(QPoint(cx, cy));
			break;
		}
		case BOT:{
			if(l->y2() - l->y1() != 0)
				ratio = (double)(cy - l->y1()) / (double)(l->y2() - l->y1());
			p = QRect(l->p1()-QPoint(margin,margin), l->p2()+QPoint(margin,margin))
						.contains(QPoint(cx, cy));
			break;
		}
		}
		// itt biztos??tom, hogy l ??rt??ke megfelel?? legyen amikor kil??p??nk a ciklusb??l:
		if(!p)
			++l;
	}
	if(p && OUTlinedivisionRatio != nullptr)
		*OUTlinedivisionRatio = ratio;
	
	return l;
}
void LineComponent::connectionAllowed(std::weak_ptr<CanvasElement> connectorOwnerOwner, std::weak_ptr<Connector> connSrc, QPoint p, std::weak_ptr<Plug> plug){
	auto observedSrc = connectorOwnerOwner.lock();
	Target t(0,0,std::shared_ptr<CanvasElement>());
	if(observedSrc == nullptr){
		t = Target(p.x(), p.y(), observedSrc);
	}
	else{
		t = Target(p.x() - observedSrc.get()->x(), p.y() - observedSrc.get()->y(), connectorOwnerOwner);
	}
	auto plObs = plug.lock();
	assert(plObs);
	auto connObs = connSrc.lock();
	assert(connObs);
	if(plObs == pl1){
		if(auto observedStart = start.canvasElement.lock()){
			pl1->detach();
		}
		start = t;
		pl1->attach(*connObs.get());
		if(!lineSegments.empty()){
			attachToElement(lineSegments.begin(), START, connSrc, t);
		}
	}
	else{
		if(auto observedEnd = end.canvasElement.lock()){
			pl2->detach();
		}
		end = t;
		pl2->attach(*connObs.get());
		if(!lineSegments.empty()){
			attachToElement(std::prev(lineSegments.end()), END, connSrc, t);
		}
	}
	if(observedSrc){
	}
}
void LineComponent::copyFrom(const CanvasElement* o){
	CanvasElement::copyFrom(o);
	const LineComponent* olc = dynamic_cast<const LineComponent*>(o);
	assert(olc);
	
	draggedLinePart = olc->draggedLinePart;
	margin = olc->margin;
	lineSegments = olc->lineSegments;
	draggingLineIt = lineSegments.end();
	/// start nem ??ll??tom be;
	/// end nem ??ll??tom be;
	///  pl1 nem ??ll??tom be
	///  pl2 nem ??ll??tom be
	/// 
	/// connRel1 nem ??ll??tom be
	/// connRel2 nem ??ll??tom be
	/// connTB1  nem ??ll??tom be
	/// connTB2  nem ??ll??tom be
}

void LineComponent::connectToInitialConnectors(){
	if(auto startObserved = start.canvasElement.lock()){
		//forwardPlugSig(pl1.get());
		startObserved->tryConnToThis(weak_from_this(), start.getPoint_FreshInCanvas(), pl1);

	}
  else{
    qDebug("no start ce");
  }
	if(auto endObserved = end.canvasElement.lock()){
		//forwardPlugSig(pl2.get());
		endObserved->tryConnToThis(weak_from_this(), end.getPoint_FreshInCanvas(), pl2);

	}
  else{
  }
	recreateLineSegments();
	//arrangeConns();
	//emit linesChanged_CEW();

}
void LineComponent::connectorTranslationChanged(std::weak_ptr<Connector> srcConn, std::weak_ptr<Plug> srcPlug, QPoint newTr){
	(void) srcConn;
	(void) srcPlug;
	//(void) prevTr;
	(void) newTr;
	reconnect();
}
void LineComponent::connectorDirectionChanged(std::weak_ptr<Connector> srcConn, std::weak_ptr<Plug> srcPlug, Direction newDir){
	(void) srcConn;
	(void) srcPlug;
	//(void) prevDir;
	(void) newDir;
	reconnect();
}
void LineComponent::connectorAreaChanged(std::weak_ptr<Connector> srcConn, std::weak_ptr<Plug> srcPlug, QRect newArea){
	(void) srcConn;
	(void) srcPlug;
	(void) newArea;
	reconnect();
}

TextComponent::TextComponent(Model* model_, QRect geometry_) 
	: 
	  CanvasElement(model_, CanvasElement_Type::TEXTBOXT, geometry_),
	  pl(std::make_shared<Plug>(this))
{
	plugs.push_back(pl);
	//forwardPlugSig(pl.get());
}
TextComponent::~TextComponent(){
	
	
}

std::weak_ptr<Plug> TextComponent::getMainPlug() const{
	return pl;
}
void TextComponent::CEW_textChanged(QString newt){
	text = newt;
}
void TextComponent::connectionAllowed(std::weak_ptr<CanvasElement> ceT, std::weak_ptr<Connector> conn, QPoint p, std::weak_ptr<Plug> plug){
	(void) p;

	emit ConnectedTo_C(weak_from_this(), plug, ceT, conn);
}
void TextComponent::copyFrom(const CanvasElement* o){
	CanvasElement::copyFrom(o);
	const TextComponent* otc = dynamic_cast<const TextComponent*>(o);
	assert(otc);
	
	text = otc->text;
	/// pl-t nem ??ll??tom be
	letterSpacing = otc->letterSpacing;
	charSize = otc->charSize;
}


std::ostream& TextComponent::save(std::ostream& os){
	CanvasElement::save(os);
	writeWSpacedStr(os, text);
	saveSharedPtr(os, pl);
	os << letterSpacing << std::endl;
	os << charSize << std::endl;
	return os;
};

std::istream& TextComponent::load(std::istream& is){
	CanvasElement::load(is);
	readWSpacedStr(is, text);
	loadSharedPtr(is, pl, this);
	is >> letterSpacing;
	is >> charSize;
	return is;
}

std::ostream& TextComponent::saveAggregations(std::ostream& os){
	CanvasElement::saveAggregations(os);
	pl->saveAggregations(os);
	return os;
}

std::istream& TextComponent::loadAggregations(std::istream& is){
	CanvasElement::loadAggregations(is);
	pl->loadAggregations(is);
	return is;
		
	}
void TextComponent::connectorTranslationChanged(std::weak_ptr<Connector> srcConn, std::weak_ptr<Plug> srcPlug, QPoint newTr){
	(void) srcConn;
	(void) srcPlug;
  geometry.translate(newTr-geometry.topLeft());
}
void TextComponent::connectorAreaChanged(std::weak_ptr<Connector> srcConn, std::weak_ptr<Plug> srcPlug, QRect newArea){
	(void) srcConn;
	(void) srcPlug;
  geometry = newArea;
}
QRect mrect(QPoint p0, int w, int h){
	return QRect(p0.x(),p0.y(), w,h).normalized();
}


UMLClassComponent::UMLClassComponent(Model* model_, QRect geometry_) 
	:
		CanvasElement(model_, CanvasElement_Type::UMLCLASST, geometry_),
    pl(std::make_shared<Plug>(this)),
		connTextTitle(std::make_shared<Connector>(this, CanvasElement_Type::TEXTBOXT)),
		connTextFields(std::make_shared<Connector>(this, CanvasElement_Type::TEXTBOXT)),
		connTextMethods(std::make_shared<Connector>(this, CanvasElement_Type::TEXTBOXT)),
		connLeft(std::make_shared<Connector>(this, LEFT, CanvasElement_Type::LINET)),
		connRight(std::make_shared<Connector>(this, RIGHT, CanvasElement_Type::LINET)),
		connTop(std::make_shared<Connector>(this, TOP, CanvasElement_Type::LINET)),
		connBottom(std::make_shared<Connector>(this, BOT, CanvasElement_Type::LINET))
{
  plugs.push_back(pl);
  connectors.push_back(connTextTitle);
  connectors.push_back(connTextFields);
  connectors.push_back(connTextMethods);
  connectors.push_back(connLeft);
  connectors.push_back(connRight);
  connectors.push_back(connBottom);
  connectors.push_back(connTop);
}

std::weak_ptr<Plug> UMLClassComponent::getMainPlug() const{
	return pl;
}
std::ostream& UMLClassComponent::saveAggregations(std::ostream& os){
	CanvasElement::saveAggregations(os);
	
	pl->saveAggregations(os);
	connTextTitle->saveAggregations(os);
	connTextFields->saveAggregations(os);
	connTextMethods->saveAggregations(os);
	connLeft->saveAggregations(os);
	connRight->saveAggregations(os);
	connTop->saveAggregations(os);
	connBottom->saveAggregations(os);
	return os;
}
std::istream& UMLClassComponent::loadAggregations(std::istream& is){
	CanvasElement::loadAggregations(is);
	
	pl->loadAggregations(is);
	connTextTitle->loadAggregations(is);
	connTextFields->loadAggregations(is);
	connTextMethods->loadAggregations(is);
	connLeft->loadAggregations(is);
	connRight->loadAggregations(is);
	connTop->loadAggregations(is);
	connBottom->loadAggregations(is);
	return is;
}
std::ostream& UMLClassComponent::save(std::ostream& os){
	CanvasElement::save(os);
	
	const std::string umlToken = "UMLClassComponent";
		os << umlToken << std::endl;
		
	saveSharedPtr(os, pl);
	saveSharedPtr(os, connTextTitle);
	saveSharedPtr(os, connTextFields);
	saveSharedPtr(os, connTextMethods);
	os << textHeight << std::endl;
	saveSharedPtr(os, connLeft);
	saveSharedPtr(os, connRight);
	saveSharedPtr(os, connTop);
	saveSharedPtr(os, connBottom);
	return os;
}
std::istream& UMLClassComponent::load(std::istream& is){
	CanvasElement::load(is);
	//ellen??rz??m, hogy t??nyleg umlclasscomponent adatai olvasom-e:
	const std::string umlToken = "UMLClassComponent";
	std::string checkStr = "";
		is >> checkStr; assert(checkStr == umlToken);
	
	loadSharedPtr(is, pl, this);
	loadSharedPtr(is, connTextTitle, this, CanvasElement_Type::TEXTBOXT);
	loadSharedPtr(is, connTextFields, this, CanvasElement_Type::TEXTBOXT);
	loadSharedPtr(is, connTextMethods, this, CanvasElement_Type::TEXTBOXT);
	is >> textHeight;
	loadSharedPtr(is, connLeft, this, CanvasElement_Type::LINET);
	loadSharedPtr(is, connRight, this, CanvasElement_Type::LINET);
	loadSharedPtr(is, connTop, this, CanvasElement_Type::LINET);
	loadSharedPtr(is, connBottom, this, CanvasElement_Type::LINET);
	return is;
}

std::weak_ptr<Connector> UMLClassComponent::getConnTitle() const{
	return connTextTitle;
}
std::weak_ptr<Connector> UMLClassComponent::getConnFields() const{
	return connTextFields;
}
std::weak_ptr<Connector> UMLClassComponent::getConnMethods() const{
	return connTextMethods;
}
std::weak_ptr<Connector> UMLClassComponent::getConnLeft() const{
	return connLeft;
}
std::weak_ptr<Connector> UMLClassComponent::getConnRight() const{
	return connRight;
}
std::weak_ptr<Connector> UMLClassComponent::getConnTop() const{
	return connTop;
}
std::weak_ptr<Connector> UMLClassComponent::getConnBottom() const{
	return connBottom;
}
void UMLClassComponent::copyFrom(const CanvasElement* o){
	CanvasElement::copyFrom(o);
	const UMLClassComponent* occ = dynamic_cast<const UMLClassComponent*>(o);
	assert(occ);
	
	textHeight = occ->textHeight;
	/// pl-t nem ??ll??tom be
	/// connTextTitle-t nem ??ll??tom be    
	/// connTextFields-t nem ??ll??tom be    
	/// connTextMethods-t nem ??ll??tom be  	   
	/// connLeft-t nem ??ll??tom be          
	/// connRight-t nem ??ll??tom be        
	/// connTop-t nem ??ll??tom be           
	/// connBottom-t nem ??ll??tom be        
}


UMLRelationship::UMLRelationship(Model* model_, CanvasElement_Type rtype_, QRect geometry_) : 
		CanvasElement(model_, rtype_, geometry_),
		dir(Direction::TOP),
		pl(std::make_shared<Plug>(this))
{
	plugs = {pl};
	connectors = {};
}

std::weak_ptr<Plug> UMLRelationship::getMainPlug() const{
	return pl;
}
CanvasElement_Type UMLRelationship::getType() const{
	return CEtype;
}
void UMLRelationship::connectorTranslationChanged(std::weak_ptr<Connector> srcConn, std::weak_ptr<Plug> srcPlug, QPoint newTr){
	(void) srcConn;
	(void) srcPlug;
  geometry.translate(newTr-geometry.topLeft());
}
void UMLRelationship::connectorDirectionChanged(std::weak_ptr<Connector> srcConn, std::weak_ptr<Plug> srcPlug, Direction newDir){
	(void) srcConn;
	(void) srcPlug;
	auto srcObs = srcConn.lock();
	assert(srcObs);
	dir = newDir;
}
void UMLRelationship::connectorAreaChanged(std::weak_ptr<Connector> srcConn, std::weak_ptr<Plug> srcPlug, QRect newArea){
	(void) srcPlug;
	auto srcObs = srcConn.lock();
	assert(srcObs);
	dir = srcObs.get()->getDir();
  geometry = newArea;
}
void UMLRelationship::connectionAllowed(std::weak_ptr<CanvasElement> connectorOwner, std::weak_ptr<Connector> srcConn, QPoint p, std::weak_ptr<Plug> plug){
	(void) p;
	if(auto srcObs = srcConn.lock()){
		dir = srcObs.get()->getDir();
		emit ConnectedTo_C(weak_from_this(), plug, connectorOwner, srcConn);
	}
}
Direction UMLRelationship::getDir() const{
	return dir;
}  
void UMLRelationship::copyFrom(const CanvasElement* o){
	CanvasElement::copyFrom(o);
	const UMLRelationship* orm = dynamic_cast<const UMLRelationship*>(o);
	assert(orm);
	
	dir = orm->dir;
	length = orm->length;
	thickness = orm->thickness;
	/// pl-t nem ??ll??tom be   
}
std::ostream& UMLRelationship::save(std::ostream& os){
	CanvasElement::save(os);
	saveSharedPtr(os, pl);
	os << dir << " " << length << " " << thickness << std::endl;
	
	return os;
}
std::istream& UMLRelationship::load(std::istream& is){
	CanvasElement::load(is);
	loadSharedPtr(is, pl, this);
	
	int dirInt; is >> dirInt;
	dir = (Direction)dirInt;
	is >> length >> thickness;
	
	return is;
}
std::ostream& UMLRelationship::saveAggregations(std::ostream& os){
	CanvasElement::saveAggregations(os);
	pl->saveAggregations(os);
	return os;
}
std::istream& UMLRelationship::loadAggregations(std::istream& is){
	CanvasElement::loadAggregations(is);
	pl->loadAggregations(is);
	return is;
}

