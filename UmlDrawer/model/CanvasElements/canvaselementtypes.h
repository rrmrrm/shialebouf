#ifndef CANVASELEMENTNTTYPES_H
#define CANVASELEMENTNTTYPES_H
#include <QString>
#include <QObject>
#include <set>
#include <map>
/*
enum CanvasElement_Type{
	ERROR,
	LINE,
	TEXTBOX,
	UMLCLASS,
	
	RELATIONSHIP
	//AGGREGATION,
	//COMPOSITION,
	//EXTENSION,
	//CIRCLE
enum Relationship_Type{
	ERROR_RT,
	AGGREGATION,
	COMPOSITION,
	EXTENSION,
	CIRCLE
};
};*/
// ábra-osztály azonosítók:
// (a vonal felismerése nem neurális hálóval történik)
enum CanvasElement_Type{ // CE_flatType
	  TEXTBOXT =0
	, CIRCLET =1
	, COMPOSITIONT =2
	, AGGREGATIONT=3
	, UMLCLASST=4
	, EXTENSIONT=5
  // DIRECTEDASSOCT makes NN unreliable
  //, DIRECTEDASSOCT=6
	, LINET=6
	, ERROR = 404
};
namespace CanvasElementTypeNames{
// for DISPLAYING CanvasElement_Type-s. the contents will be translated
inline std::map<CanvasElement_Type, QString> CE_map_type_name{
  {TEXTBOXT, QObject::tr("Textbox")},
  {CIRCLET, QObject::tr("Ownership_dot")},
  {COMPOSITIONT, QObject::tr("Composition")},
  {AGGREGATIONT, QObject::tr("Aggregation")},
  {UMLCLASST, QObject::tr("UML_class")},
  {EXTENSIONT, QObject::tr("Extension")},
  //{DIRECTEDASSOCT, QObject::tr("Directed_association")},
  {LINET, QObject::tr("Line")},
  {ERROR, QObject::tr("Error")}
};
};
namespace CanvasElementTypeGroups{
	inline std::set<CanvasElement_Type> RELATIONSHIPS {AGGREGATIONT, COMPOSITIONT, EXTENSIONT, CIRCLET/*, DIRECTEDASSOCT*/};
}

enum CanvasManipulationResult {	
	DRAG_UNSELECTED, STOPDRAGGING, NOTHING, 
	ELEMENTSELECTED_RESULT, ELEMENT_DESELECTED_RESULT, ELEMENTPRESSED_RESULT, ELEMENT_DBLPRESSED_RESULT,
	ELEMENT_STARTRESIZING_RESULT, ELEMENT_RESIZING_RESULT, ELEMENT_STOPRESIZING_RESULT,
	Line,
	GestureType, GesturePointType
};
#endif // CANVASELEMENTNTTYPES_H
