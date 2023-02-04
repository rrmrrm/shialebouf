#include "neuralclassifier.h"
#include "shape_drawing/drawing.h"
#include "persistence/persistence.h"

#include <QDebug>

#include <functional>
#include <chrono>
#include <thread>
#include <string>
#include <iostream>
#include <fstream>
namespace {
std::string data_end_token = "_COMPLETE_";
std::string session_end_token = "_DO_SHUTDOWN_";
std::string classifier_started_token = "_CLASSIFIER_STARTED_";
std::string classifierLogPath = "classifierLog.txt";

void waitUntil(std::function<bool()> pred, int tryTimes, int waitmsec){
	while(tryTimes > 0 && !pred()){
		qDebug() << ".";
		std::this_thread::sleep_for(std::chrono::milliseconds(waitmsec));
		--tryTimes;
	}
}
}
NeuralClassifier::NeuralClassifier(uint resHorizontal_, uint resVertical_)
	: resHorizontal(resHorizontal_)
	, resVertical(resVertical_)
{
  persistence = Persistence::GetInstance();
	qDebug("NeuralClassifier::NeuralClassifier(..): opening toCpp and toPy files");
  
  // if this app crashed, the next time we run it there will be an existing classifier process reading from toPy.txt, so we tell it to stop
  {
  std::ofstream dezombificator("toPy.txt", std::ios_base::app );
  dezombificator << session_end_token << std::endl << std::flush;
  dezombificator.close();
  }
  // and wait so it has time to read the shutdown message
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  
  // clear stuff from the files, that has been left in the previous run of the the processes
  {
  std::ofstream emptyfier1("toCpp.txt", std::ios_base::trunc );
  emptyfier1.close();
  std::ofstream emptyfier2("toPy.txt", std::ios_base::trunc );
  emptyfier2.close();
  }
	in.open("toCpp.txt"); 
	out.open("toPy.txt", std::ios_base::trunc ); //? , std::ios::app
  // and wait so the files will be emtpied
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
	if(!in.is_open())
	{
		persistence->accessLogger()->warn("NeuralClassifier::NeuralClassifier(..): couldn't open infile");
	}
	if(!out.is_open()){
		persistence->accessLogger()->warn("NeuralClassifier::NeuralClassifier(..): couldn't open outfile");
	}
  // run classifier paralell to this program
  classifierRunner = new std::thread([this](){
    persistence->runClassifier({});
  });
  // poll until classifier is ready
  int tryTimes = 60;
	int waitMsec = 500;
  bool successfulRead;
  std::string greeting;
  persistence->accessLogger()->log("waiting for classifier component..");
  waitUntil(
        [this, &successfulRead, &greeting]{
          //need to clear eofbit or failbit because previous failed reads set it
          in.clear();
          successfulRead = bool(in >> greeting);
          return successfulRead;
        }
        , tryTimes
        , waitMsec);
  if(!successfulRead){
    // classifier didnt send anything
    persistence->accessLogger()->warn("NeuralClassifier::NeuralClassifier(..): classifier didnt send the greeting message in time");
    return;
  }
  if(greeting != classifier_started_token){
    // classifier sent something else instead of the classifier_started_token
    persistence->accessLogger()->warn(
          str("NeuralClassifier::NeuralClassifier(..): classifier sent '")
          + greeting
          + str("' instead of '") 
          + classifier_started_token 
          + str("' as greeting message"));
    return;
  }
  persistence->accessLogger()->log("classifier component started");
}
NeuralClassifier::~NeuralClassifier(){
  if(out.is_open()){
    out << session_end_token << std::endl << std::flush;
    out.close();
  }
	in.close();
  qDebug("NeuralClassifier::~NeuralClassifier(): waiting for classifier process to exit..");
  classifierRunner->join();
  delete classifierRunner;
  qDebug("classifier exited.");
  persistence->releaseInstance();
}

std::ostream& send2DVecflattened(std::ostream& os, const std::vector< std::vector<double> >& pixelMatrix){
	bool first = true;
	if(pixelMatrix.size() == 0){
		qDebug("warning: send2DVecflattened: pixelMatrix.size() is 0. skip");
		return os;
	}
	//os << pixelMatrix.size()*pixelMatrix[0].size() << std::endl;
	for(int i = 0 ; i < (int)pixelMatrix[0].size() ; ++i){
		for(int j = 0 ; j < (int)pixelMatrix.size() ; ++j){
			if(first){
				first = false;
			}
			else{
				os << ',';
			}
			os << pixelMatrix.at(j).at(i);
		}		
	}
	// data_end_token jelzi a fogadó félnek, hogy teljes az adat, már használhatja:
	os << data_end_token << std::endl << std::flush;
	return os;
}
bool readAnswer(std::ifstream& in, std::vector<double>& answer){
  //need to clear eofbit or failbit because previous failed reads set it
	in.clear();
	answer.clear();
	double readVal = 0;
	bool p = true;
	bool readSomething = false;
	while(p){
		p = bool(in >> readVal);
		if(p){
			p = true;
			readSomething = true;
			answer.push_back(readVal);
		}
	}
	return readSomething;
}
void printVec(std::ostream& os, const std::vector<double>& v){
	for(auto e : v){
		os << e << ", ";
	}
	os << std::endl << std::flush;
}
CanvasElement_Type NeuralClassifier::classify(Drawing* d){
	qDebug("NeuralClassifier::classify: called");
  std::locale::global(std::locale::classic());
  //https://github.com/nlohmann/json/issues/51
	assert(d);
	assert(d->getResolutionHorizontal() == resHorizontal);
	assert(d->getResolutionVertical() == resVertical);
	if(!send2DVecflattened(out, d->getPixelMatrix())){
		persistence->accessLogger()->warn(str("NeuralClassifier::classify(..): failed to send pixelMatrix to out file."));
    return CanvasElement_Type::ERROR;
		//exit(EXIT_FAILURE);
	}
	std::vector<double> answer;
	int tryTimes = 50;
	int waitMsec = 200;
	bool successfulRead = false;
	waitUntil(
		[this, &answer, &successfulRead]{
			successfulRead = readAnswer(in, answer);
			return successfulRead;
		}
		, tryTimes
		, waitMsec
	);
	if(!successfulRead){
    persistence->accessLogger()->warn(str("NeuralClassifier::classify: no answer from classifier check calssifier log at '") + classifierLogPath + str("'"));
    return CanvasElement_Type::ERROR;
	}
	printVec(std::cout, answer);
	//assert(answer.size() == )
	int maxActivationIndex = 0;
	double max = std::numeric_limits<double>::min();
	for(uint i = 0 ; i < answer.size() ; ++i){
		std::string debStr = "outPut(" + std::to_string(i) + "): " + std::to_string(answer[i]);
		qDebug("%s", debStr.c_str());
		if(answer[i] > max){
			maxActivationIndex = i;
			max = answer[i];
		}
	}
	return (CanvasElement_Type)maxActivationIndex;
}

