#ifndef IDRAWINGCLASSIFIER_H
#define IDRAWINGCLASSIFIER_H
#include "model/CanvasElements/canvaselementtypes.h"
#include <string>
class Drawing;
class IDrawingClassifier
{
public:
	IDrawingClassifier();
	virtual ~IDrawingClassifier();
public:
  // if calssify returns CanvasElement_Type::ERROR check lastError's conent.
  // otherwise classify returns the drawing-class that it thinks 'd' is a member of.
	virtual CanvasElement_Type classify(Drawing* d) = 0;
};

#endif // IDRAWINGCLASSIFIER_H
