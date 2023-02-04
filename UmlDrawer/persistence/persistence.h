#ifndef Persistence_H
#define Persistence_H

#include "persistence/logger.h"// irasi/olvasasi hibak kiirasahoz
//#include "../model/identityDir/connectiondata.h"
#include "../model/CanvasElements/canvaselement.h"
#include "network/network.h"// neurális háló
#include "shape_drawing/drawing.h"
#include "streamoperators.h"
#include "common.hpp"
#include <QString>
#include <QDir>
#include <QDirIterator>

#include <iostream>
#include <fstream>
#include <sstream>
#include <assert.h>
#include <variant>
#ifdef TESTING 
#define  privatet public
#define  protectedt public
#else
#define  privatet private
#define  protectedt protected
#endif
typedef std::variant<bool*, unsigned int*, int*,float*,double*,std::string*> ptr_variants;
///Singleton Perzisztencia
///1. dolga jól olvasható filebaírást megvalósítani.
///2.: részletes, a javítást könnyítő hibaüzenetek gyártása(írási/olvasási hiba esetén) a Logger osztály felhasználásával
/// az egyes adattagok kiírása előtt kiírja azok nevét, hogy 
/// a későbbi beolvasás során egyértelmű legyen, ha rosz változót írtunk be, és a hibaüzenet tartalmazni is fogja mely változónál volt eltérés,
///  ezért könnyebb lesz a hibaüzenet alapján javítani a hibát.
class Model;
class Persistence
{
///fields:
privatet:
  static Persistence* InnerObject;
  static uint refCounter;
	
	std::ifstream writer;
	std::ofstream reader;
	//NetworkData* nd = nullptr;
	NeuralNetwork* n = nullptr;
	
	///fields for iterating through a directory:
	bool iterating = false;
	//QDirIterator* dirIterator = nullptr;
  QStringList  files;
  QDir qdir;
  int fileIndIndDir = 0;
	std::ifstream drawingReader;
	QString skipHavingSuffix;
  std::string pythonCommand = "py";
	std::string trainerPath = "trainer.py";
	std::string trainerLogPath = "trainLog.txt";
  std::string classifierPath = "classifier.py";
  std::string classifierLogPath = "classifierLog.txt"; 
  Logger* l = nullptr;
public:
	Logger* accessLogger() const;
/// ctors, dtor, operators:
public:
	static Persistence* GetInstance();
	void releaseInstance();
privatet:
	Persistence();
	~Persistence();

/// other methods:
privatet:
	bool isOpen(std::ifstream& fs, const std::string methodName, const QString& nameTag, bool errOnFail=false);
	bool isOpen(std::ofstream& fs, const std::string methodName, const QString& nameTag, bool errOnFail=false);
	
	///
	/// \brief readCheckNameTagSep
	///		Ha a nameTag argumentum üres, akkor csak a szeparátort meglétét ellenőrzi.
	///		Amúgy beolvas egy stringet, ellenőrzi, hogy nametaggel megegyezik-e. ha nem egyezik , akkor hibás futást jelez.
	///		Ha ez sikeres akkor a szeparátor meglétét is ellenőrzi.
	/// 
	///		Ha errOnFail igaz, a hibás futás jelzése azt jelenti, hogy assert-el hibát dob a program.
	///		Ha hamis, akkor hibás futás eseten csak hamisat adunk vissza.
	/// \param ifs
	/// \param methodName
	/// \param nameTag: azonosító szöveg hibajavítás segítését szolgálja.
	/// \param errOnFail
	/// \return 
	///
	bool readCheckNameTagSep(std::ifstream& ifs, const std::string methodName, const QString& nameTag, bool errOnFail=false);
	bool writeNameTagSep(std::ofstream& ofs, const std::string methodName, const QString& nameTag, bool errOnFail=false);
	template<class fStreamType>
	bool isStreamValid(fStreamType& fs, const std::string methodName, const QString& nameTag, bool errOnFail=false){
		if(fs.fail()){
			std::stringstream msg;
			msg	<< "ERROR: " << methodName << ": found nametag, but failed while reading/writing variable: "
				<< nameTag.toStdString();
			if(errOnFail){
				l->err(msg.str());
				l->flush();
				assert(!msg.str().c_str());
			}
			else{
				l->warn(msg.str());
				return false;
			}
		}
		return true;
	}
	
	///
	/// \brief loadVariable
	/// ha nametag üres, akkor arra számítunk, hogy a fileban nincs nametag és szeparátor se
	/// \param ifs
	/// \param data
	/// \param nameTag
	/// \param errOnFail
	/// \return 
	///
	template<class T>
	bool loadVariable(std::ifstream& ifs, T& data, QString nameTag, const std::string& methodName, bool errOnFail=false){
		//ha errOnFail igaz és pl isOpen(..)-ben hiba történik, akkor a logikai kifejezés nem értékelődik ki, mert közben hibát dobunk
		if(isOpen(ifs, methodName, nameTag, errOnFail)
				&& readCheckNameTagSep(ifs, methodName, nameTag, errOnFail)
				&& (ifs >> data || true)
				&& isStreamValid(ifs, methodName, nameTag, errOnFail) 
		){
			return true;
		}
		else{
			return false;
		}
	}
	template<class T>
	bool loadVariable(std::ifstream& ifs, T* data, QString nameTag, const std::string& methodName, bool errOnFail=false){
		assert(data);
		//ha errOnFail igaz és pl isOpen(..)-ben hiba történik, akkor a logikai kifejezés nem értékelődik ki, mert közben hibát dobunk
		if(isOpen(ifs, methodName, nameTag, errOnFail)
				&& readCheckNameTagSep(ifs, methodName, nameTag, errOnFail)
				&& (ifs >> *data || true)
				&& isStreamValid(ifs, methodName, nameTag, errOnFail) 
		){
			return true;
		}
		else{
			return false;
		}
	}
	///
	/// \brief saveVariable
	///		ha nameTag üres, akkor csak a data-t írjuk ki, a nameTagot (nyilván) nem, és ekkor szeparátort sem írunk a fileba.
	/// \param ofs
	/// \param data
	/// \param nameTag
	/// \param errOnFail
	/// \return 
	///
	template<class T>
	bool saveVariable(std::ofstream& ofs, const T& data, QString nameTag, const std::string& methodName, bool errOnFail=false){
		//ha errOnFail igaz és pl isOpen(..)-ben hiba történik, akkor a logikai kifejezés nem értékelődik ki, mert közben hibát dobunk
		if(isOpen(ofs, methodName, nameTag, errOnFail)
				&& writeNameTagSep(ofs, methodName, nameTag, errOnFail)
				&& (ofs << data << std::endl || true)
				&& isStreamValid(ofs, methodName, nameTag, errOnFail) 
		){
			return true;
		}
		else{
			return false;
		}
	}
  void loadVars(
				const std::vector<std::pair<QString, ptr_variants> >&,
        const std::string& methodName,
				const std::string& file
				);
public:
	
	bool saveCEs(QString fileNam, std::list<MyCompositionT<CanvasElement>>& canvasElements);
	bool loadCEs(Model* model, QString fileName, std::list<MyCompositionT<CanvasElement>>& canvasElements);

	int getDrawingCategoriesNumInDir(QString dir){
		QDirIterator it(dir, QDir::Files);
		int categNum = 0;
		while(it.hasNext()){
			QString fileName = it.next();
			if(it.fileInfo().suffix()!="csv")
				++categNum;
		}
		return categNum;
	}
	void startIterationInDir(QString dir, QString skipHavingSuffix_ = ""){
		skipHavingSuffix = skipHavingSuffix_;
    qdir = QDir(dir);
    qdir.setSorting(QDir::SortFlag::Name);
    files = qdir.entryList(QStringList(), QDir::Files);
    fileIndIndDir = 0;
	}
	bool nextDrawingCategoryInDir(){
		//if(!dirIterator)
		//	return false;
		
		if(drawingReader.is_open())
			drawingReader.close();
		
		bool foundNext = false;
    QString fileName;
		while(!foundNext && fileIndIndDir < files.size()/*dirIterator->hasNext()*/){
			fileName = files.at(fileIndIndDir);
      ++fileIndIndDir;
      //QString fileName = dirIterator->next();
			if(		skipHavingSuffix == "" 
					//|| dirIterator->fileInfo().suffix() != skipHavingSuffix
          ||  !fileName.endsWith(skipHavingSuffix)
            )
			{
				foundNext = true;
			}
		}
		if(foundNext){
			//std::string path = dirIterator->filePath().toStdString();
      std::string path = qdir.filePath(fileName).toStdString();
			drawingReader.open(path, std::ios_base::in);
			if(!drawingReader.is_open()){
				std::stringstream ss;
				ss << "Persistence::nextDrawingCategoryInDir(): found next category's file(" << path << "), but couldn't open it";
				l->warn(ss.str());
			}
			return drawingReader.is_open();
		}
		else{
			return false;
		}
	}
	QString getActualFileName(){
    if(fileIndIndDir >= files.size())
      return "NOFILE";
    return files.at(fileIndIndDir);
	}
	QString getActualFilePath(){
    if(fileIndIndDir >= files.size())
      return "NOFILE";
    return qdir.filePath(files.at(fileIndIndDir));
	}
	int getDrawingNumInCategory(int newResHor, int newResVert);
	Drawing* nextDrawingInCategory(int newResHor, int newResVert);
  bool loadConfig(
      const std::vector<std::pair<QString, ptr_variants> >&,
      const std::string& methodName,
      const std::string& file = CONF_FILE_NAME);
	bool saveNetwork();
	bool saveDrawingImages(const QString& fName, std::vector<Drawing*> drawings);
	// sysCommand: execute a system command, and wait for the return value
  int sysCommand(std::string command);
  bool runTrainer(const std::vector<std::string>&);
  void runClassifier(const std::vector<std::string>& args);
};


#endif // Persistence_H
