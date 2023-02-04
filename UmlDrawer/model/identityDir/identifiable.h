#ifndef IDENTIFIABLE_H
#define IDENTIFIABLE_H

#include <vector>
#include <iostream>

typedef long IdType;
class Identifiable
{
	IdType id;
	static IdType nextId;
public:
  static std::vector<IdType> ids;
	static std::vector<Identifiable*> existingObjects;
	static const IdType INVALID_ID;//= -1;
	
protected:
	Identifiable(IdType);
public:
	Identifiable();
	//Identifiable& operator=(const Identifiable& o){}
	Identifiable(const Identifiable& o) = delete;
	Identifiable(Identifiable&& o);
	virtual ~Identifiable();
	
private:
	static IdType requestId();
	IdType freeId();
  /// ellenőrzi, hogy az id szabad-e. Ha nem, akkor hibát dob, amúgy visszaadja forcedId-t
	/// ha validateId igaz, akkor azt is ellenőrzi, hogy forcedId != INVALID_ID
	IdType forceGetId(IdType, bool validateId = false);
	//static IdType setNextId();
protected:
	//void save_chainCallThis(std::ostream&);
	//void load_chainCallThis(std::istream&);
public:
  IdType getId() const;
	virtual std::ostream& save(std::ostream&);
	virtual std::istream& load(std::istream&);
	static Identifiable* findById(IdType);
  // tells Identftifiable to only set ids above 'restrict'. 
  // doesn't change behaviour of forceGetId
  // this when used in OPersistence::load makes sure loaded ids(which are registered with forceGetId) dont collide with automatically given ids
  static void restrictAutomaticIdsAbve(IdType restrict);
  // gets biggest id, to use with restrictAutomaticIdsAbve method
  static IdType getTopId();
};


#endif // IDENTIFIABLE_H
