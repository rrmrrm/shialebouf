#include "persistence.h"
#include "common.hpp"
#include "../model/model.h"
//#include "../model/identityDir/connectiondata.h"
#include "../model/CanvasElements/canvaselement.h"
#include "../model/CanvasElements/canvaselementbase.h"
#include "../model/CanvasElements/canvaselementtypes.h"
#include "../model/identityDir/identifiable.h"

#include <fstream>
#include <sstream>
#include <variant>
#include <thread>

Persistence* Persistence::InnerObject = nullptr;
uint Persistence::refCounter = 0;
Persistence::~Persistence(){
  delete l;
  if(n != nullptr)
    delete n;
}

bool Persistence::isOpen(std::ifstream& fs, const std::string methodName, const QString& nameTag, bool errOnFail){
	if(!fs.is_open()){
		std::stringstream msg;
		msg << methodName << ": file hadn't been opened. variable name:"
			<< nameTag.toStdString();
		if(errOnFail){
			l->err(msg.str());
			l->flush();
			exit(1);//assert(!msg.str().c_str());
		}
		else{
			l->warn(msg.str());
			return false;
		}
	}
	return true;
}
bool Persistence::isOpen(std::ofstream& fs, const std::string methodName, const QString& nameTag, bool errOnFail){
	if(!fs.is_open()){
		std::stringstream msg;
		msg	<< methodName << ": file hadn't been opened. variable name:"
			<< nameTag.toStdString();
		if(errOnFail){
			l->err(msg.str());
			l->flush();
      std::cout << "isopen: debug1" << std::endl << std::flush;
			exit(1); //assert(!msg.str().c_str());
      std::cout << "isopen: debug2" << std::endl << std::flush;
		}
		else{
			l->warn(msg.str());
			return false;
		}
	}
	return true;
}
bool Persistence::readCheckNameTagSep(std::ifstream& ifs, const std::string methodName, const QString& nameTag, bool errOnFail){
	std::string readNameTag = "";
	/// Ha nameTag üres, akkor azt várjuk, hogy a fileban sincs benne, így az ellenőrzését kihagyjuk. 
	/// Ha nem üres, akkor összevetjük a file ketkez szavával:
	if(nameTag != "" && !nameTag.isEmpty()){
		ifs >> readNameTag;
		if(ifs.fail() || readNameTag != nameTag.toStdString()){
			std::stringstream msg;
      if(readNameTag.empty()){
        msg	<< methodName << ": not found nametag(and likely variable for): '" 
          << nameTag.toStdString();
      }
      else{
        msg	<< methodName << ": nameTag mismatch; expected: '" 
          << nameTag.toStdString()
          << "', but in file: '"
          << readNameTag
          << "'";
      }
			if(errOnFail){
				l->err(msg.str());
				l->flush();
				exit(1); //assert(!msg.str().c_str());
			}
			else{
				l->warn(msg.str());
				return false;
			}
		}
		std::string separator;
		if(!(ifs >> separator)){
			std::stringstream msg;
			msg	<< methodName << ": nameTag found, but separator is missing";
			if(errOnFail){
				l->err(msg.str());
				l->flush();
				exit(1); //assert(!msg.str().c_str());
			}
			else{
				l->warn(msg.str());
				return false;
			}
		}
	}
	return true;
}
bool Persistence::writeNameTagSep(std::ofstream& ofs, const std::string methodName, const QString& nameTag, bool errOnFail){
	//ha nameTag üres, akkor kiírjuk és a szeparátort is:
	if(nameTag != "" && !nameTag.isEmpty()){
		ofs << nameTag.toStdString() << std::endl;
		if(ofs.fail()){
			std::stringstream ss;
			ss << methodName << ": couldn't write '"
				<< nameTag.toStdString()
				<< "' nametag to file";
			if(errOnFail){
				l->err(ss.str());
				l->flush();
				exit(1); //assert(!ss.str().c_str());
			}
			else{
				l->warn(ss.str());
				return false;
			}
		}
		if(!(ofs << " : " << std::endl)){
			std::stringstream ss;
			ss << methodName << ": wrote nametag, but couldn't write separator to file";
			if(errOnFail){
				l->err(ss.str());
				l->flush();
				exit(1); //assert(!ss.str().c_str());
			}
			else{
				l->warn(ss.str());
				return false;
			}
		}
	}
	return true;
}
// QString CONNS_TOKEN = "HERE_BE_CONNECTION_DATA";
// bool Persistence::saveModelConnectionsAppend(std::ofstream& osConns, const std::list<ConnectionData>& modelConnections){
//   if(!isOpen(osConns, "Persistence::saveModelConnectionsAppend", CONNS_TOKEN, false))
//     return false;
//   if(!writeNameTagSep(osConns, "Persistence::saveModelConnectionsAppend", CONNS_TOKEN, false))
//     return false;
// 	saveContainer(osConns, ConnectionData::modellIntraconnections);
//   return true;
// }
// bool Persistence::loadModelConnections(std::ifstream& isConns, std::list<ConnectionData>& modelConnections){
//   if(!isOpen(isConns, "Persistence::loadModelConnections", CONNS_TOKEN, false))
//     return false;
//   if(!readCheckNameTagSep(isConns, "Persistence::loadModelConnections", CONNS_TOKEN, false))
//     return false;
// 	loadContainer(isConns, ConnectionData::modellIntraconnections);
//   return true;
// }
// bool Persistence::saveModelConnectionsAppend(const std::string& fileName, const std::list<ConnectionData>& modelConnections){
//   std::ofstream osConns(fileName);
//   if(!isOpen(osConns, "Persistence::saveModelConnectionsAppend", CONNS_TOKEN, false))
//     return false;
//   if(!writeNameTagSep(osConns, "Persistence::saveModelConnectionsAppend", CONNS_TOKEN, false))
//     return false;
// 	saveContainer(osConns, ConnectionData::modellIntraconnections);
//   return true;
// }
// bool Persistence::loadModelConnections(const std::string& fileName, std::list<ConnectionData>& modelConnections){
//   std::ifstream isConns(fileName);
//   if(!isOpen(isConns, "Persistence::loadModelConnections", CONNS_TOKEN, false))
//     return false;
//   if(!readCheckNameTagSep(isConns, "Persistence::loadModelConnections", CONNS_TOKEN, false))
//     return false;
// 	loadContainer(isConns, ConnectionData::modellIntraconnections);
//   return true;
// }

Logger* Persistence::accessLogger() const{
  return l;
}
Persistence* Persistence::GetInstance(){
	if(InnerObject == nullptr)
		InnerObject = new Persistence();
	++refCounter;
	return InnerObject;
}
void Persistence::releaseInstance(){
	if( refCounter <= 0)
		return;
	--refCounter;
  if(refCounter == 0 && InnerObject != nullptr){
		delete InnerObject;
    InnerObject = nullptr;
  }
}

Persistence::Persistence(){
  l = new Logger();
  //load stuff for persistence
  std::ifstream ifs(PERS_CONF_FILE_NAME);
  //DO NOT change from "Persistence::Persistence()"! tesztjegyzőkönyv így várja el...
  loadVariable(ifs, pythonCommand, nameOf(pythonCommand), "Persistence::Persistence()", true);
  loadVariable(ifs, trainerPath, nameOf(trainerPath), "Persistence::Persistence()", true);
  loadVariable(ifs, trainerLogPath, nameOf(trainerLogPath), "Persistence::Persistence()", true);
  
  loadVariable(ifs, classifierPath, nameOf(classifierPath), "Persistence::Persistence()", true);
  loadVariable(ifs, classifierLogPath, nameOf(classifierLogPath), "Persistence::Persistence()", true);
  ifs.close();
}
namespace {
//QString CEsNoAggrToken = "CEs_without_aggregations";
const std::string CEtoken = "CanvasElement";
const QString topIdToken = "TopId";
}
bool Persistence::saveCEs(QString fileName, std::list<MyCompositionT<CanvasElement>>& canvasElements){
	std::ofstream os(fileName.toStdString());
	/// save contained objects:
  if(!saveVariable(os, Identifiable::getTopId(), topIdToken, "Persistence::saveCEs"))
     return false;
  //if(!isOpen(os, "Persistence::saveCEs", CEsNoAggrToken, false))
  //  return false;
  //if(!writeNameTagSep(os, "Persistence::saveCEs", CEsNoAggrToken, false))
  //  return false;
  
	os << canvasElements.size() << std::endl;
	for(auto& it: canvasElements){
		os << CEtoken << std::endl;
		if(it.get()){
			os << it->CEtype << std::endl;
			it->save(os);
		}
		else{
			qDebug() << "warning: Model::saveCEs: canvasElement is nullptr";
			os << it;
		}
	}
	os << canvasElements.size() << std::endl;
	for(auto& it: canvasElements){
		os << CEtoken << std::endl;
		assert(it.get());
		if(it.get()){
			it->saveAggregations(os);
		}
	}
  // if(!saveModelConnectionsAppend(os, ConnectionData::modellIntraconnections))
  //   return false;
	// os << std::endl;
	// os << "modellIntraconnections" << std::endl;
  return true;
}
bool Persistence::loadCEs(Model* model, QString fileName, std::list<MyCompositionT<CanvasElement>>& canvasElements){
  std::ifstream is(fileName.toStdString());
  IdType topLoadedId;
  if(!loadVariable(is, topLoadedId, topIdToken, "Persistence::saveCEs"))
    return false;
  // make sure loaded ids don't collide with loaded ones
  Identifiable::restrictAutomaticIdsAbve(topLoadedId);
  
  //if(!isOpen(is, "Persistence::loadCEs", CEsNoAggrToken, false))
  //  return false;
  //if(!readCheckNameTagSep(is, "Persistence::loadCEs", CEsNoAggrToken, false))
  //  return false;
  // // need clear ces otherwise load would fail as ids collide
  // model->canvasElements.clear();
	canvasElements.clear();
  //if(!is.is_open()){
  //  l->err(str("Persistence::loadCEs: couldn't open '") + fileName.toStdString() + str("' file"));
  //}
	std::string checkstr = "";
	int csiz = 0; is>> csiz; 
  if(!is){
    l->err(str("Persistence::loadCEs: couldn't read CanvasElement's count from '") + fileName.toStdString() + str("' file"));
    return false;
  }
	for(int i = 0 ; i < csiz ; ++i){
		is >> checkstr; assert(checkstr == CEtoken);
		CanvasElement_Type cet = ERROR;
		/// load CE type info into cet enum:
		int e=0; is >> e; cet=(CanvasElement_Type)e;
		assert(cet != CanvasElement_Type::ERROR);
		///create CE with the type according to cet, and rt enums:
		{
		auto newCE = CanvasElement::createCanvasElement(model, cet, QRect(0,0,100,100));
    // ha nem sikerult letrehozni, hamissal terunk vissza:
    if(!newCE)
      return false;
		///load CE:
		newCE->load(is);
		// add CE to canvasElements:
		canvasElements.push_back(std::move(newCE));
		}
	}
	int aggrInfoNum; 
	is >> aggrInfoNum;
	///ellenőrzöm, hogy is és is ugyanannyie elem információját tartalmazza-e:
	///is-ben canvasElements.size() darab elem adatának kell lennie:
	assert(aggrInfoNum == (int)canvasElements.size());
	
	for(auto& it: canvasElements){
		is >> checkstr; assert(checkstr == CEtoken);
		assert(it.get());
		if(it.get()){
			it->loadAggregations(is);
		}
	}
	//std::string connPartTok;
	//is >> connPartTok;
	//assert(connPartTok == "modellIntraconnections");
  
  // if(!loadModelConnections(is, ConnectionData::modellIntraconnections))
  //   return false;
  return true;
}
void Persistence::loadVars(
		const std::vector<std::pair<QString, ptr_variants> >& vars,
    const std::string& methodName,
		const std::string& file
		)
{
	std::ifstream ifs(file);
	for(auto var : vars){
    QString& varName = var.first;
		std::visit(
				[this, &ifs, varName, methodName](auto& v){
					loadVariable(ifs, v, varName, methodName, true);
				}, 
				var.second
			);
	}
	ifs.close();
}

int Persistence::getDrawingNumInCategory(int newResHor, int newResVert){
  if(/*!dirIterator ||*/!drawingReader.is_open())
    return 0;
  std::ifstream ifs(getActualFilePath().toStdString());
  int categNum = 0;
  Drawing d;
  while(d.load(ifs, newResHor, newResVert/*, true,true*/)){
    ++categNum;
  }
  return categNum;
}
Drawing* Persistence::nextDrawingInCategory(int newResHor, int newResVert){
  if(/*!dirIterator || */!drawingReader.is_open())
    return nullptr;
  Drawing* ret = Drawing::createDrawingFromFile(drawingReader, newResHor, newResVert);
  return ret;
}
bool Persistence::loadConfig(
    const std::vector<std::pair<QString, ptr_variants> >& vars,
    const std::string& methodName,
		const std::string& file){
  // load things for model
  loadVars(vars, methodName, file);
  // load things for persistence:
  std::ifstream ifs(file);
  //loadVariable(ifs, pythonCommand, nameOf(pythonCommand), methodName, true);
  //loadVariable(ifs, trainerPath, nameOf(pythonCommand), methodName, true);
  //loadVariable(ifs, trainerLogPath, nameOf(pythonCommand), methodName, true);
  ifs.close();
  return true;
}
bool Persistence::saveNetwork(){
  if(n == nullptr){
    l->warn("Persistence::saveNetwork(): couldn't save network, because network was null'");
    return false;
  }
  std::ofstream ofs("trainedNetwork.txt");
  bool succ =
      saveVariable(ofs, n->layers, "", "Persistence::saveNetwork()")
      && saveVariable(ofs, n->weightMatrices, "", "Persistence::saveNetwork()");
  ofs.close();
  if(!succ){
    l->err(str("Persistence::saveNetwork(): couldn't save network to '")+str("trainedNetwork.txt")+str("' file"));
    exit(1);
  }
  return n;
}
bool Persistence::saveDrawingImages(const QString& dirName, std::vector<Drawing*> drawings){
	if( !QDir(dirName).exists() ){
		return false;
	}
	QDir dir(dirName);
	for(int i = 0 ; i < (int)drawings.size() ; ++i){
		QString formatExt("PNG");
		std::string newFileName =  std::to_string(i)+'.'+formatExt.toLower().toStdString();
		QString newFilePath(dir.filePath(newFileName.c_str()));
		if(!drawings[i]->accessImage().save(newFilePath, formatExt.toUpper().toStdString().c_str())){
			return false;
		}
	}
	return true;
}
int Persistence::sysCommand(std::string command){
	//Checking if processor is available:
	if (!system(NULL)){
    l->warn(str("Persistence::sysCommand(command) failed, because processor was unavailable. command was '" + command + str("'")));
		return 2;
	}
	
	//Executing command;
	int i;
	i = system(command.c_str());
	return i;
}
bool Persistence::runTrainer(const std::vector<std::string>& args){
	std::string command = 
		pythonCommand + str(" ")
		+ trainerPath + str(" ");
	// paraméterek összefűzése a 'command' stringbe
	for(int i = 0 ; i < (int)args.size() ; i++){
		command += args[i] +str(" ");
	}
	// trainer output mentése log fileba
	command += str(" > ") + trainerLogPath;// + str("\n") + str(";pause");
	
	return sysCommand(command.c_str()) == EXIT_SUCCESS;
}
void Persistence::runClassifier(const std::vector<std::string>& args){
	std::string command = 
		pythonCommand + str(" ")
		+ classifierPath + str(" ");
	// paraméterek összefűzése a 'command' stringbe
	for(int i = 0 ; i < (int)args.size() ; i++){
		command += args[i] +str(" ");
	}
	// trainer output mentése log fileba
	command += str(" > ") + classifierLogPath;// + str("\n") + str(";pause");
  
  auto ret = sysCommand(command.c_str());
  if(ret != 0 && l){
    l->warn(str("Persistence::runClassifier: the command '") + command + str("' returned ") + std::to_string(ret));
    std::cerr << "Persistence::runClassifier: the command '" << command << "' returned " << ret << std::endl;
  }
	//return sysCommand(command.c_str()) == EXIT_SUCCESS;
}
