#include "logger.h"

#include <QString>
#include <QDir>
#include <QDateTime>

//#include <common.hpp>
#include <iostream>
#include <fstream>
#include <sstream>

// macro machination
// if stringstitute is called with kartoffel, then it returns "kartoffel"
#define stringstitute(krumpli) #krumpli
// if EXPANSE is called with substitutedHereWithItsValue  which is defined to be val, then it calls stringstitute and passes val to it.
// so both "val" will be returned
#define EXPANSE(substitutedHereWithItsValue) stringstitute(substitutedHereWithItsValueWithItsValue)                                                                                                                                                                           // easy, it only took half an our to find and understand this

const QString Logger::logfileDirName = "logfiles";
#ifdef LOG_DIR
const QString Logger::logfileDirName= EXPANSE(LOG_DIR);
#endif


void Logger::my_assert(bool expr, QString failMsgPart1, QString failMsgPart2, QString failMsgPart3, QString failMsgPart4){
  if(!expr){
    QString qstring = failMsgPart1
      + failMsgPart2
      + failMsgPart3
      + failMsgPart4;
    assert(!qstring.toStdString().c_str());
  }
}
Logger::Logger()
{
  logOsFileName = "log.log";
  warnOsFileName = "warnings.log";
  errOsFileName = "errors.log";
  
  QDir dir;
  if( !dir.mkpath(logfileDirName) ){
    std::cerr << "ERROR: Logger::Logger(): could not create '" <<  logfileDirName.toStdString() << "' directory" << std::endl;
    exit(1);/// az egyes adattagok kiírása előtt kiírja azok nevét, hogy 
    /// a későbbi beolvasás során egyértelmű legyen, ha rosz változót írtunk be, és a hibaüzenet tartalmazni is fogja mely változónál volt eltérés,
    ///  ezért könnyebb lesz a hibaüzenet alapján javítani a hibát.
  }
  /// a log fileba eredeti tartalmához hozzáfűzzük az újakat,
  /// a hiba és figyelmeztetés tartalmát viszont fölülírjuk
  /// filenyitasi hiba eseten a program hibat jelez a cerr-en es kiszall
  std::string logPath = QDir(logfileDirName).filePath(QString::fromStdString(logOsFileName)).toStdString();
  std::string warnPath = QDir(logfileDirName).filePath(QString::fromStdString(warnOsFileName)).toStdString();
  std::string errPath = QDir(logfileDirName).filePath(QString::fromStdString(errOsFileName)).toStdString();
  logOs.open(logPath, std::ios_base::app);
  warnOs.open(warnPath);
  errOs.open(errPath);
  if(!logOs.is_open()){
    std::cerr << "ERROR: Logger::Logger(): could not open '" <<  logPath << "' file" << std::endl;
    exit(1);
  }
  logOs << std::endl;
  printTimeStamp(logOs);
  logOs << "program started" << std::endl;
  
  if(!warnOs.is_open()){
    std::cerr << "ERROR: Logger::Logger(): could not create '" <<  warnPath << "' file" << std::endl;
    exit(1);
  }
  if(!errOs.is_open()){
    std::cerr << "ERROR: Logger::Logger(): could not create '" << errPath << "' file" << std::endl;
    exit(1);
  }
  
}
Logger::~Logger(){
  logOs.close();
  warnOs.close();
  errOs.close();
}
void Logger::printTimeStamp(std::ostream& os){
  QDateTime qDT = QDateTime::currentDateTime();
  os << qDT.date().toString().toStdString() << " ";
  os << qDT.time().hour() << " ";
  os << qDT.time().minute() << " ";
  os << qDT.time().second() << " ";
  os << std::endl;
}
void Logger::flush(){
  logOs.flush();
  warnOs.flush();
  errOs.flush();
}
bool Logger::log(std::string msg){
  std::lock_guard<std::mutex> lg(logOsMutex);
  std::cout << msg << std::endl;
  logOs << msg << std::endl;
  auto ret = !logOs.fail() && !logOs.bad();
  return ret;
}
bool Logger::warn(std::string msg){
  std::lock_guard<std::mutex> lg(warnOsMutex);
  std::cout << "WARNING: " << msg << std::endl;
  std::cerr << "WARNING: " << msg << std::endl;
  warnOs << msg << std::endl;
  flush();
  auto ret = !warnOs.fail() && !warnOs.bad();
  return ret;
}
bool Logger::err(std::string msg){
  std::lock_guard<std::mutex> lg(errOsMutex);
  std::cout << "ERROR happened. check the error log: '" << errOsFileName << "'" << std::endl;
  std::cerr << "ERROR: " << msg << std::endl;
  logOs << "ERROR happened. check the error log: '" << errOsFileName << "'" << std::endl;
  
  errOs << msg << std::endl;
  flush();
  auto ret = !errOs.fail() && !errOs.bad();
  return ret;
}
