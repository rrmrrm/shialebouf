#include "persistenceTest.h"
#include "persistence/persistence.h"

#include <QtTest/QtTest>
#include <QDir>
#include <QFile>

#include <assert.h>
#include <fstream>
//#include <unistd.h> // fork
#include <sys/wait.h> // waitpid, wifexited, wifexitstatus
#include <thread>
#include <chrono>
QTEST_MAIN(Unittest::PersistenceUnittest);

namespace Unittest{
PersistenceUnittest::PersistenceUnittest(){
  
}
// init testcase
void PersistenceUnittest::initTestCase(){
}
// cleanup testcase
void PersistenceUnittest::cleanupTestCase(){
}
// init testfunction
void PersistenceUnittest::init(){
  pers = Persistence::GetInstance();
  //TODO test refcounter an INNerObject
  //QCOMPARE(pers->refCounter, 1);
  normalStream.open(normalFileName); // i don't provide std::ios::in, because i want the file to be created
  QVERIFY(normalStream.is_open());
  
  
}
// cleanup testfunction
void PersistenceUnittest::cleanup(){
  pers->releaseInstance();
  // delete file
  QFile::remove(QString(normalFileName.c_str()));
  // close stream
  normalStream.close();
  
  // make sure nonexistent file is nonexistent
  if(QFile::exists(nonexistentFileName.c_str())){
    QFile::remove(nonexistentFileName.c_str());
  }
  //QCOMPARE(pers->refCounter, 0);
  QVERIFY(!QFile::exists(normalFileName.c_str()));
  QVERIFY(!normalStream.is_open());
  QVERIFY(!QFile::exists(nonexistentFileName.c_str()));
}
//testfunctions:

void PersistenceUnittest::isOpenOfstreamTest1()
{
   bool isOpen = pers->isOpen(normalStream, "shouldntPrintWarnings", "nametag", false);
   QVERIFY(isOpen);
}
void PersistenceUnittest::isOpenOfstreamTest2(){
  std::ofstream os(nonexistentFileName, std::ios::in | std::ios::out);// provide ios::in flag so it wont try to create the file
  bool isOpen = pers->isOpen(os, "shouldPrintWarnings", "nametag", false);
  os.close();// close os BEFORE QVERIFY just to be sure its closed
  QVERIFY(!isOpen);
}
// isOpenOfstreamTest3() is in persistenceSimpletes project
}
