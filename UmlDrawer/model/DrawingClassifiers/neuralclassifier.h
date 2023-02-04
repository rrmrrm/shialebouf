#ifndef NEURALCLASSIFIER_H
#define NEURALCLASSIFIER_H

#include "idrawingclassifier.h"
#include <QString>
#include <fstream>
#include <thread>
class Persistence;
class NeuralClassifier : public IDrawingClassifier
{
	Persistence* persistence;
	uint resHorizontal;
	uint resVertical;
	std::ifstream in;
	std::ofstream out;
  std::thread* classifierRunner = nullptr;
public:
	NeuralClassifier(uint resHorizontal, uint resVertical);
  // tells the classifier to quit
	~NeuralClassifier();
public:
	CanvasElement_Type classify(Drawing* d) override;
};

#endif // NEURALCLASSIFIER_H
