#ifndef AGGREGATIONT_H
#define AGGREGATIONT_H

#include "identityDir/identifiable.h"

#include <iostream>
#include <memory>
template<class T>
class AggregationT
{
private:
	std::weak_ptr<T> wp;
	int aggregateId {-1};
	
public:
	//myweak_ptr();
	AggregationT();
	AggregationT(const std::shared_ptr<T>&  shared);
	//constexpr myweak_ptr() noexcept = default;
	AggregationT(const AggregationT<T>&);
	//AggregationT(const AggregationT<T>& __r);
	AggregationT(AggregationT<T>&& __r);
	AggregationT<T>& operator=(const AggregationT<T>& __r);
	//operator=(const weak_ptr<_Yp>& __r) noexcept
	AggregationT<T>& operator=(const std::shared_ptr<T>& __r);
	AggregationT<T>& operator=(AggregationT<T>&& __r);
	
	AggregationT(const std::weak_ptr<T>&);
	//AggregationT(const AggregationT<T>& __r);
	AggregationT(std::weak_ptr<T>&& __r);
	//operator std::weak_ptr<T>();
	operator std::weak_ptr<T>() const;
	bool operator==(std::weak_ptr<T> o) const;
	bool operator!=(std::weak_ptr<T> o) const;
private:
	void setAggregateId(const std::shared_ptr<T>&  shared);
	void setAggregateId(const std::weak_ptr<T>&  shared);
public:
	std::shared_ptr<T> lock() const;
	void reset();
	IdType getAggregateId() const;
	virtual std::ostream& save(std::ostream& os) const;
	virtual std::istream& load(std::istream& is, Identifiable* aggregateOwner = nullptr);
};

#include "aggregationt_impl.h"

#endif // AGGREGATIONT_H
