#ifndef COMPOSITIONT_H
#define COMPOSITIONT_H

#include <memory>
#include <iostream>
#include <assert.h>
///
/// \brief The MyCompositionT class
/// MyCompositionT olyan shared_ptr, aminek csak egy referenciája lehet (a lockolt weak ptr-eket leszámítva)
/// ezt úgy érem, el, hogy a másoló konstruktort törlöm.
template<class T>
class MyCompositionT : public std::shared_ptr<T>
{
private:				   
	template<class TDer>
	MyCompositionT(const MyCompositionT<TDer>& o){
	   /// a teamplatelt =delete deklarácó tiltja meg a templatelt copy ctor használatát valamiért.
	   ///Ugzhogz priváttá tettem a copzctort, és emellett itt egy assert(false):
	   assert(false);
	}					
public:
	MyCompositionT() :std::shared_ptr<T>()
	{}
	
	template<class TDer>
	MyCompositionT() :std::shared_ptr<TDer>()
	{}

	MyCompositionT(const MyCompositionT<T>& o) =delete;
	
	template<class TDer>
	MyCompositionT(MyCompositionT<TDer>&& o) :std::shared_ptr<TDer>(std::move(o))
	{}
	template<class TDer>
	MyCompositionT(std::shared_ptr<TDer>&& o) :std::shared_ptr<T>(std::move(o))
	{}
	
	virtual ~MyCompositionT()
	{}
	
	template<class TDer>
	MyCompositionT<T>& operator=(const MyCompositionT<TDer>& o){
		///egyszerre csak egy menedzselő smartPointer lehet
		assert(o == nullptr);
		std::shared_ptr<TDer>::operator=(o);
		return *this;
	}
	template<class TDer>
	MyCompositionT<T>& operator=(const std::shared_ptr<TDer>& o){
		assert(o == nullptr);
		std::shared_ptr<TDer>::operator=(o);
		return *this;
	}
	template<class TDer>
	MyCompositionT<T>& operator=(MyCompositionT<TDer>&& o){
		std::shared_ptr<TDer>::operator=(std::move(o));
		/// jelezni kell shared_ptr -nek, hogy átvegye o menedzselését.(move nélkül másolná)
		assert(o == nullptr);
		return *this;
	}
	template<class TDer>
	MyCompositionT<T>& operator=(std::shared_ptr<TDer>&& o){
		/// jelezni kell shared_ptr -nek, hogy átvegye o menedzselését.(move nélkül másolná)
		std::shared_ptr<TDer>::operator=(std::move(o));
		assert(o == nullptr);
		return *this;
	}
	
	operator std::shared_ptr<T>(){
		return *static_cast<std::shared_ptr<T>*>(this);
	}
};

#endif // COMPOSITIONT_H
