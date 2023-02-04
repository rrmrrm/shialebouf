#ifndef AGGREGATIONT_IMPL_H
#define AGGREGATIONT_IMPL_H

#include "aggregationt.h"
#include "identityDir/identifiable.h"

//#include "model/CanvasElements/canvaselementbase.h"

#include <QDebug>

#include <memory>
#include <type_traits>
#include <assert.h>

template<class T> 
AggregationT<T>::AggregationT() : wp()
{}
template<class T> 
AggregationT<T>::AggregationT(const std::shared_ptr<T>&  shared): wp(shared)
{
	static_assert( std::is_base_of_v<Identifiable, T> == true );
	setAggregateId(shared);
	if(aggregateId == Identifiable::INVALID_ID){
		//qDebug("warning: AggregationT<T>::AggregationT(const std::shared_ptr<T>&  shared): aggregate id is invalid");
	}
}
template<class T> 
AggregationT<T>::AggregationT(const AggregationT<T>& o) : wp(o.wp), aggregateId(o.getAggregateId())
{
	static_assert( std::is_base_of_v<Identifiable, T> == true );
}
template<class T> 
AggregationT<T>::AggregationT(AggregationT<T>&& o ) 
{
	static_assert( std::is_base_of_v<Identifiable, T> == true );
	wp = std::move(o.wp);
	aggregateId = o.getAggregateId();
	o.aggregateId = Identifiable::INVALID_ID;
}


template<class T> 
AggregationT<T>& AggregationT<T>::operator=(const AggregationT<T>& o){
	wp = o.wp;
	aggregateId = o.getAggregateId();
	return *this;
}
template<typename T>
AggregationT<T>& AggregationT<T>::operator=(const std::shared_ptr<T>& shared){
	wp = shared;
	setAggregateId(shared);
	return *this;
}
template<typename T>
AggregationT<T>& AggregationT<T>::operator=(AggregationT<T>&& o){
	wp = std::move(o.wp);
	aggregateId = o.getAggregateId();
	o.aggregateId = Identifiable::INVALID_ID;
	return *this;
}

template<typename T>
AggregationT<T>::AggregationT(const std::weak_ptr<T>& stdweak) :wp(stdweak)
{
	static_assert( std::is_base_of_v<Identifiable, T> == true );
	setAggregateId(stdweak);
	if(aggregateId == Identifiable::INVALID_ID){
		//qDebug("warning: AggregationT<T>::AggregationT(const std::weak_ptr<T>& stdweak): aggregate id is invalid");
	}
}
template<typename T>
AggregationT<T>::AggregationT(std::weak_ptr<T>&& o)
{
	static_assert( std::is_base_of_v<Identifiable, T> == true );
	setAggregateId(o);
	wp = std::move(o);
	if(aggregateId == Identifiable::INVALID_ID){
		//qDebug("warning: AggregationT<T>::AggregationT(std::weak_ptr<T>&& o): aggregate id is invalid");
	}
}
template<typename T>
AggregationT<T>::operator std::weak_ptr<T>() const{
	return wp;
}
template<typename T>
bool AggregationT<T>::operator==(std::weak_ptr<T> o) const{
	return wp.lock() == o.lock();
}
template<typename T>
bool AggregationT<T>::operator!=(std::weak_ptr<T> o) const{
	return wp.lock() != o.lock();
}

template<typename T>
void AggregationT<T>::setAggregateId(const std::shared_ptr<T>&  shared){
	static_assert( std::is_base_of_v<Identifiable, T> == true );
	std::shared_ptr<Identifiable> sId = std::static_pointer_cast<Identifiable>(shared);
	if(sId){
		aggregateId = sId->getId();
	}
	else{
		//qDebug("warning: AggregationT<T>::setAggregateId(const std::shared_ptr<T>&  shared): aggregate id is invalid");
		aggregateId = Identifiable::INVALID_ID;
	}
}
template<typename T>
void AggregationT<T>::setAggregateId(const std::weak_ptr<T>&  weak){
	static_assert( std::is_base_of_v<Identifiable, T> == true );
	auto shared = weak.lock();
	
	if(!shared){
		//qDebug("warning: AggregationT<T>::setAggregateId(const std::weak_ptr<T>&  weak): aggregate id is invalid");
		aggregateId = Identifiable::INVALID_ID;
	}
	else{
		std::shared_ptr<Identifiable> sharedIdent = std::static_pointer_cast<Identifiable>(shared);
		assert(sharedIdent);
		aggregateId = sharedIdent->getId();
	}
}
template<typename T>
std::shared_ptr<T> AggregationT<T>::lock() const{
	return wp.lock();
}
template<class T>
void AggregationT<T>::reset(){
	wp.reset();
}
template<typename T>
IdType AggregationT<T>::getAggregateId() const{
	return aggregateId;
}
template<typename T>
std::ostream& AggregationT<T>::save(std::ostream& os) const{
	if(wp.lock()){
		os << "weak_nonnull" << std::endl;
		os << aggregateId << std::endl;
	}
	else{
		os << "weak_null" << std::endl;
	}
	return os;
}
/// egy  AggregationT objektum (nevezzük 'B'-nek) 2 objektum közötti aggregációt  képvisel:
/// a 'B'-t tartalmazó 'A' objektum aggregálja a 'B' által mutatott 'C' objektumot.
/// 'B' aggregáció csak akkor tölthető be(akkor hozható létre) ha 'C' egy létező shared_ptr.
/// 
/// így lehet hogy több olyan objektumot kell egymás után betölteni, amik tartalmaznak AggregationT-eket(tehát az általam épülő mentés-betöltés rendszer által felismerhető módon aggregálnak más objektumokat),
///  ha ezekbe az aggregációk kört alkotnak, akkor nem lehet semelyik objektumot se először teljesen betölteni,
///  viszont két fázisban be lehet tölteni az objektunokat:
///		1.: betöltjük az objektumokat, de az aggregációkat nem töltjük be. e mellett be kell tölteni minden aggregált objektumot is a második fázishoz. 
///		2.: az aggregált objektumokat töltjük be.(a AggregationT-eket)
template<typename T>
std::istream& AggregationT<T>::load(std::istream& is, Identifiable* aggregateOwner){
	std::string validity;
	is >> validity;
	assert(is);
	
	if(validity == "weak_nonnull"){
		is >> aggregateId;
		assert(is);
		Identifiable* found = Identifiable::findById(aggregateId);
		if(!found){
			qDebug() << "WARNING: AggregationT<T>::load(..): load failed: object with id doesnt exist. maybe it wasnt loaded yet\n";
			if(aggregateOwner){
				qDebug() << "\taggregateOwner's id: " << aggregateOwner->getId() << "/n";
			}
			qDebug() << "\taggregated object's id: " << getAggregateId() << "/n";
			return is;
		}
		T* foundAsT = dynamic_cast<T*>(found);
		if(!foundAsT){
			qDebug() << "WARNING: AggregationT<T>::load(..): load failed: object with exists, BUT cannot be converted to T\n";
			if(aggregateOwner){
				qDebug() << "\taggregateOwner's id: " << aggregateOwner->getId() << "/n";
			}
			qDebug() << "\taggregated object's id: " << getAggregateId() << "/n";
			return is;
		}
		auto foundES = dynamic_cast<std::enable_shared_from_this<T>*>(foundAsT);
		if(!foundES){
			qDebug() << "WARNING: AggregationT<T>::load(..): load failed: object with id exists, AND can be converted to T, BUT is not derived from std::enable_shared_from_this<T>!\n";
			if(aggregateOwner){
				qDebug() << "\taggregateOwner's id: " << aggregateOwner->getId() << "/n";
			}
			qDebug() << "\taggregated object's id: " << getAggregateId() << "/n";
			return is;
		}
		auto foundAsTShared = foundAsT->shared_from_this();
		wp = foundAsTShared;
	}
	else{
		wp.reset();
	}
	return is;
}
#endif // AGGREGATIONT_IMPL_H
