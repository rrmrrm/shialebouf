#pragma once

#define TESTING
#include "persistence/persistence.h"
#include <QObject>


namespace Unittest{
class PersistenceUnittest: public QObject
{
	Q_OBJECT
  Persistence* pers = nullptr;
  std::string normalFileName = "normalFile.txt";
  std::ofstream normalStream;
  std::string nonexistentFileName = "nonexistentfilename.txt";
public:
	PersistenceUnittest();
private slots:
    // notes:
    // A testcase is a group of testfunctions.
    // QVERIFY(condition) stops skips the rest of the test if the condition is false;
    // so it behaves like a gtest ASSERT_TRUE.
  
    // init testcase
    void initTestCase();
    // cleanup testcase
    void cleanupTestCase();
    // init testfunction
    void init();
    // cleanup testfunction
    void cleanup();
    
    //
	  //testfunctions:
    //	
    
  // testing private API:
    void isOpenOfstreamTest1();
    void isOpenOfstreamTest2();
    // isOpenOfstreamTest3() is in persistenceSimpletes project
    
    // void isOpenIfstreamTest1();
    // Persistence();
    // ///TODO: valamiert debugban az alkalmazasbol kiszallasokr(jobbfelul xgombra klikk) eseten nem hivodik meg a destruktor (legalabbis nem all meg a progi a breakpointon ) itt. Kijavitani
    // ~Persistence();
    // bool readCheckNameTagSep(std::ifstream& ifs, const std::string methodName, const QString& nameTag, bool errOnFail=false);
    // bool writeNameTagSep(std::ofstream& ofs, const std::string methodName, const QString& nameTag, bool errOnFail=false);
    // template<class fStreamType>
    // bool isStreamValid(fStreamType& fs, const std::string methodName, const QString& nameTag, bool errOnFail=false){
    // template<class T>
    // bool loadVariable(std::ifstream& ifs, T& data, QString nameTag, bool errOnFail=false){
    // template<class T>
    // bool loadVariable(std::ifstream& ifs, T* data, QString nameTag, bool errOnFail=false){
    // template<class T>
    // bool saveVariable(std::ofstream& ofs, const T& data, QString nameTag, bool errOnFail=false){
    // void loadVars(
    //       const std::vector<std::pair<QString, ptr_variants> >&,
    //       const std::string& file = CONF_FILE_NAME
    //       );
    
    
  //testing public API:
    //static Persistence* GetInstance();
    //void releaseInstance();
    //NeuralNetwork* getNetwork();
    //
    //void saveCEs(QString fileNam, std::list<CanvasElementContainerT>& canvasElements);
  
    //void loadCEs(Model* model, QString fileName, std::list<CanvasElementContainerT>& canvasElements);
    //void saveModelConnectionsAppend(QString fileName, const std::list<ConnectionData>& modelConnections);
    //void loadModelConnections(QString fileName, std::list<ConnectionData>& modelConnections);
    //
    //int getDrawingCategoriesNumInDir(QString dir);
    //void startIterationInDir(QString dir, QString skipHavingSuffix_ = "");
    //bool nextDrawingCategoryInDir();
    //QString getActualFileName();
    //QString getActualFilePath();
    //int getDrawingNumInCategory(int newResHor, int newResVert);
    //Drawing* nextDrawingInCategory(int newResHor, int newResVert);
    //bool loadConfig(
    //    const std::vector<std::pair<QString, ptr_variants> >&,
    //    const std::string& file = CONF_FILE_NAME);
    //bool saveNetwork();
    //bool saveDrawings(const QString& fName, std::vector<Drawing*> drawings);
    //bool saveDrawingImages(const QString& fName, std::vector<Drawing*> drawings);
    //bool runTrainer(const std::vector<std::string>&);
    
};
}
