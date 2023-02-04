#include "identifiable.h"

#include <QDebug>

#include <fstream>

IdType Identifiable::nextId = 0;
std::vector<IdType> Identifiable::ids = std::vector<IdType>();
std::vector<Identifiable*> Identifiable::existingObjects;
const IdType Identifiable::INVALID_ID = -1;


Identifiable::Identifiable(IdType forcedId)
	:id(forceGetId(forcedId)) 
{
	existingObjects.push_back(this);
	/// hogy ne ütközhessen az így beállított id 
	/// egy később automatikusan kiosztott id-vel:
	if(nextId < forcedId)
		nextId = forcedId + 1;
}
Identifiable::Identifiable()
	:id(requestId()) 
{
	existingObjects.push_back(this);
}
Identifiable::Identifiable(Identifiable&& o)
	:id( forceGetId(o.freeId(), true) ){//forceGetId(.., true): validáljuk o.id-t mielőtt ez az objektum átvanné id-t.
}
Identifiable::~Identifiable(){
	auto found = std::find(existingObjects.begin(),
						   existingObjects.end(),
						   this);
  // kiszedem a ConnectionData-it a tárolóból. a qt majd megszüntetni, vagy már meg si szüntette az objektum kapcsolatait
	assert(found != existingObjects.end());
  
  // bool didEraseOne = true;
  // // addig próbálok törölni elemeket amíg találok újat.
  // while(didEraseOne){
  //   auto foundConnData = std::find_if(ConnectionData::modellIntraconnections.begin(),
  //             ConnectionData::modellIntraconnections.end(),
  //             [this](const ConnectionData& it){
  //               return it.srcId == id || it.dstId == id;
  //             });
  //   if(foundConnData != ConnectionData::modellIntraconnections.end()){
  //     ConnectionData::modellIntraconnections.erase(foundConnData);
  //   }
  //   else{
  //     // nem találtam új elemet, tehát nem keresek tovább
  //     didEraseOne = false;
  //   }
  // }
	*found = nullptr;
	existingObjects.erase(found);
	freeId();
	//freeId(id);
}

IdType Identifiable::requestId(){
	IdType next = nextId;
	bool p = true;
	while(p){
		auto found = std::find(ids.begin(), ids.end(), next);
		p = (found != ids.end());
		++next;
	}
	if(!p){
		--next;
	}
  if(next == 9){
    qDebug();
  }
	ids.push_back(next);
	nextId = next + 1;
	return next;
}
IdType Identifiable::freeId(){
	/// ha id = INVALID_ID akkor már meg lett hívva erreaz objektumra a freeId(). 
	///  ekkor nem kell semmit csinálni az eljárásban:
	if(id == INVALID_ID)
		return id;
	
	auto found = std::find(ids.begin(),ids.end(), id);
	assert(found != ids.end());
	IdType erasedId = *found;
	id = INVALID_ID;//jelezzük, hogy ennek az objektumnak már nincs az ids-tárolóban az id-je
	ids.erase(found);
	
	return erasedId;
}


IdType Identifiable::getId() const{
	return id;
}
std::ostream& Identifiable::save(std::ostream& os){
	os << id << std::endl;
	//assert(os);
	return os;
}
std::istream& Identifiable::load(std::istream& is){
	IdType newId;
	is >> newId;assert(is);
	freeId();
	id = forceGetId(newId);
	return is;
}

//void Identifiable::save(std::ostream& os){
//	save_chainCallThis(os);
//}
//void Identifiable::load(std::istream& is){
//	load_chainCallThis(is);
//}
// void Identifiable::saveIdent(std::ostream& os){
// 	Identifiable::save(os);
// }
// void Identifiable::loadIdent(std::istream& is){
// 	Identifiable::load(is);
// }
Identifiable* Identifiable::findById(IdType id){
	std::vector<Identifiable*>::iterator found = std::find_if(
				Identifiable::existingObjects.begin(),
				Identifiable::existingObjects.end(),
				[&id](Identifiable* ident){return id == ident->getId();}
	);
	if(found == Identifiable::existingObjects.end()){
		qDebug() << "WARNING: Identifiable::findById(..): no Identifiable object with " 
				 << id << " id\n";
		return nullptr;
	}
	else{
		return *found;
	}
}
void Identifiable::restrictAutomaticIdsAbve(IdType restrict){
  nextId = restrict+1;
}
IdType Identifiable::getTopId(){
  IdType max = 0;
  if(ids.size())
    max = ids[0];
  for(IdType idIt : ids){
    if(idIt > max)
      max = idIt;
  }
  return max;
}
/// TODO: foreceGetID  feltételezi, hogy id=forcegetId() formában fogják használni egy inicializáló listában,
/// pedig már nem const az id. szóval ez lehetne ezt átírhatnám setterre
IdType Identifiable::forceGetId(IdType forcedId, bool validateId){
	if(validateId){
		assert(forcedId != INVALID_ID);
	}
	auto found = std::find(ids.begin(), ids.end(), forcedId);
	assert(found == ids.end());//használt id-t nem foglalhatunk le, hibát dobunk;
	
  if(forcedId == nextId){
    ++nextId;//már itt megnöveljük nextId-t, hiszen ezt az értéket le akrarja majd a hívó foglalni
  }
	ids.push_back(forcedId);
	return forcedId;
}
