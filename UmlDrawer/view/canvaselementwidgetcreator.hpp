#ifndef CANVASELEMENTWIDGETCREATOR_H
#define CANVASELEMENTWIDGETCREATOR_H

#include "canvaselementwidgetbase.h"
#include "linewidget.h"
//#include "umlcomponentwidget.h"
#include "umlclasswidget.h"
#include "umlrelationwidget.h"
//#include "textwidget.h"
#include "textbox.h"

CanvasElementWidget* createDerived(std::weak_ptr<CanvasElement> ceb, QWidget* parent){
	auto obsCEB = ceb.lock();
	if(!obsCEB)
		return nullptr;
	switch(obsCEB->CEtype){
	case ERROR:{
		assert(!"ERROR: createDerived(..): trying to create a canvas element from CanvasManipulationResult::ERROR");
		return nullptr;
		break;
	}
	case LINET:{
		auto casted = std::dynamic_pointer_cast<LineComponent>(obsCEB);
		return new LineWidget(casted, parent);
		break;
	}
	case UMLCLASST:{
		std::shared_ptr<UMLClassComponent> casted = std::dynamic_pointer_cast<UMLClassComponent>(obsCEB);
		return new umlClassWidget(casted, parent);
		break;
	}
	case TEXTBOXT:{
		std::shared_ptr<TextComponent> casted = std::dynamic_pointer_cast<TextComponent>(obsCEB);
		//return new TextWidget(casted, parent);
    return new TextBox(casted, parent);
		break;
	}
  case EXTENSIONT:{
		std::shared_ptr<UMLExtensionSpecifier> casted = std::dynamic_pointer_cast<UMLExtensionSpecifier>(obsCEB);
		return new UmlRelationWidget(casted, parent);
		break;
	}
  // commenting out this , because cant recognise with NN properly
  //case DIRECTEDASSOCT:{
	//	std::shared_ptr<UMLDirectedAssociationSpecifier> casted = std::dynamic_pointer_cast<UMLDirectedAssociationSpecifier>(obsCEB);
	//	return new UmlRelationWidget(casted, parent);
	//	break;
	//}
	case AGGREGATIONT:{
		std::shared_ptr<UMLAggregationSpecifier> casted = std::dynamic_pointer_cast<UMLAggregationSpecifier>(obsCEB);
		return new UmlRelationWidget(casted, parent);
		break;
	}
	case COMPOSITIONT:{
		std::shared_ptr<UMLCompositionSpecifier> casted = std::dynamic_pointer_cast<UMLCompositionSpecifier>(obsCEB);
		return new UmlRelationWidget(casted, parent);
		break;
	}
	case CIRCLET:{
		std::shared_ptr<UMLCircle> casted = std::dynamic_pointer_cast<UMLCircle>(obsCEB);
		return new UmlRelationWidget(casted, parent);
		break;
	}
	default:{
		assert(!"ERROR: createDerived(..): kezeletlen CanvasElement::elementType");
		break;		
	}
	}
	return nullptr;
}

#endif // CANVASELEMENTWIDGETCREATOR_H
