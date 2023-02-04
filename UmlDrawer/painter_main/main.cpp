

bool readAndSaveGestureData = true;

#include "view/widget.h"
#include "persistence/streamoperators.h"
#include "common.hpp"
#include <QDir>
#include <QApplication>
#include <QTranslator>

#include <iostream>
#include <vector>
#include <string>
#ifdef _WIN32
// for chdir
#include <direct.h>
#include <stdio.h>
#include <stdlib.h>
#else
// for chdir
#include <unistd.h>
#endif
void printHelp(){
  std::cout << "USAGE:" << std::endl;
  std::cout << std::endl;
  std::cout << "use 'command -h' to print this help." << std::endl;
  std::cout << std::endl;
  std::cout << "if the config file is moved from the executable's directory, you have to use -p to point to the new directory of the config file:" << std::endl;
  std::cout << "'command -p path/to dir/containing the configfile'" << std::endl;
}
std::string parseArgumentsExitOnHelp(int argc, char *argv[]){
  std::string confDirSwitch = "-p";
  std::string confDir;
  // see help
  if(argc == 2){
    printHelp();
    exit(1);
  }
  if(argc >= 3){
    if(argv[1] == confDirSwitch){
      // spaces in paths are rare but they exist.
      //
      // Since there is only going to be 1 argument(which points to the directory of the config file, 
      // in which the rest of the args will be stored),
      // i can just merge the arguments
      // so the program will be able to parse this:
      // command -p dirnames with/spaces are an abomi/nation /Program Files
      for(int ai = 2 ; ai < argc ; ++ai){
        confDir += argv[ai];
        if(ai < argc-1){
          confDir += ' ';
        }
      }
    }
  }
  return confDir;
}
///TODO: tanítás előtt adatokat feldolgozásaként: outlier ábrák megjelölése. előbb megtanulni, hogyan kell ezt
/// milyen tanítóminta elemzést érdemes még végrehajtani, és hogyan kell. elemzést valszeg pzthonban könnyű
int main(int argc, char *argv[])
{
  std::string configDirPath = parseArgumentsExitOnHelp(argc, argv);
  //if(configDirPath != ""){
  //  if(chdir(configDirPath.c_str()) != 0){
  //    std::cerr << "ERROR: Couldn't switch to given directory: " << configDirPath << std::endl;
  //  }
  //}
  QDir confDir(configDirPath.c_str());
  // test if conf file is accessible at the given location:
  {
  std::string confPath = confDir
      .filePath(CONF_FILE_NAME.c_str()) // concatenate cwd with the filename
      .toStdString();// convert to std::string for to construct the ifstream
  std::ifstream testStream(confPath); 
  if(!testStream.is_open()){
    std::cerr << "ERROR: couldn't find, or open config file at" << confPath << std::endl;
    testStream.close();
    exit(1);
  }
  testStream.close();
  }
  // test if persistence conf file is accessible at configDir:
  {
  std::string persConfPath = confDir
      .filePath(CONF_FILE_NAME.c_str())
      .toStdString();
  std::ifstream testPersistenceStream(persConfPath);
  if(!testPersistenceStream.is_open()){
    std::cerr << "ERROR: couldn't find, or open persistence's config file at " << persConfPath << std::endl;
    testPersistenceStream.close();
    exit(1);
  }
  testPersistenceStream.close();
  }
	QApplication app(argc, argv);
  QTranslator translator;
  // look up hunTranslation
  if (!translator.load("hunTranslation")){    
    std::cerr << "WARNING: couldn't load 'hunTranslation' translation file"  << std::endl;
  }
  else{
    app.installTranslator(&translator);
  }
	Widget* w = new Widget();
	//QObject::connect(&app, SIGNAL(aboutToQuit()), w, SLOT(prepareForClosing()));
	w->show();
	
	return app.exec();
	
	return 0;
}
