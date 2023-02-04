
QT       += core gui 

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = project
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

CONFIG += c++17 \

SOURCES += \
    compositiont.cpp \
	model/CanvasElements/canvaselementbase.cpp \
	model/CanvasElements/canvaselement.cpp \
	model/DrawingClassifiers/idrawingclassifier.cpp \
	model/DrawingClassifiers/neuralclassifier.cpp \
	model/canvasmanipulation.cpp \
	model/connector.cpp \
	model/model.cpp \
	model/identityDir/connectiondata.cpp \
	model/identityDir/identifiable.cpp \
	model/myqline.cpp \
	model/plug.cpp \
	model/userinputmodel.cpp \
	#network/network.cpp \
	painter_main/main.cpp \
	persistence/persistence.cpp \
        persistence/logger.cpp \
	shape_drawing/drawing.cpp \
	shape_drawing/gesture.cpp \
	qrandomgenerator.cpp \
	view/canvas.cpp \
	view/canvaselementwidgetbase.cpp \
	view/connwidget.cpp \
	view/linewidget.cpp \
    view/myTextEdit.cpp \
	view/savelastdrawingwidget.cpp \
    view/textbox.cpp \
	view/umlclasswidget.cpp \
	view/umlrelationwidget.cpp \
	view/userinputdialog.cpp \
	view/widget.cpp
	
HEADERS += \
	common.hpp \
        compositiont.h \
	functions.hpp \
	model/CanvasElements/canvaselement.h \
	model/CanvasElements/canvaselementbase.h \
	model/CanvasElements/canvaselementtypes.h \
	model/DrawingClassifiers/idrawingclassifier.h \
	model/DrawingClassifiers/neuralclassifier.h \
    model/aggregationt.h \
    model/aggregationt_impl.h \
	model/canvasmanipulation.h \
	model/connector.h \
	model/dirDim.hpp \
	model/idrawingcontainer.h \
	model/initializer.hpp \
	model/model.h \
	model/identityDir/connectiondata.h \
	model/identityDir/identifiable.h \
	model/myqline.h \
	model/plug.h \
	model/target.h \
	model/userinputmodel.h \
	#network/network.h \
	persistence/persistence.h \
        persistence/logger.h \
	persistence/streamoperators.h \
	persistence/streamoperators_impl.h \
	shape_drawing/gesture.h \
	shape_drawing/drawing.h \
	qrandomgenerator.h \
	view/canvas.h \
	view/canvaselementwidgetbase.h \
	view/canvaselementwidgetcreator.hpp \
	view/connwidget.h \
	view/linewidget.h \
    view/myTextEdit.h \
	view/savelastdrawingwidget.h \
    view/textbox.h \
	view/umlclasswidget.h \
	view/umlrelationwidget.h \
	view/userinputdialog.h \
	view/widget.h
TRANSLATIONS = hunTranslation.ts

FORMS += \
        view/savelastdrawingwidget.ui \
    view/textbox.ui \
	view/umlClass.ui \
        view/userinputdialog.ui \
        view/widget.ui \
        
# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

# remove possible other optimization flags
QMAKE_CXXFLAGS_RELEASE -= -O
QMAKE_CXXFLAGS_RELEASE -= -O1
QMAKE_CXXFLAGS_RELEASE -= -O2

## add the desired -O3 if not present
#QMAKE_CXXFLAGS_RELEASE *= -O3

SUBDIRS += \
	trainer.pro
	
