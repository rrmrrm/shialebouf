QT += gui

CONFIG += c++17 console
CONFIG -= app_bundle

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

#include "model/logger.h"// irasi/olvasasi hibak kiirasahoz
#include "../model/identityDir/connectiondata.h"
#include "../model/CanvasElements/canvaselement.h"
#include "network/network.h"// neurális háló
#include "shape_drawing/drawing.h"
#include "streamoperators.h"
#include "common.hpp"

SOURCES += \
        persistenceSimpleTest.cpp \
        ../../model/CanvasElements/canvaselementbase.cpp \
        ../../model/CanvasElements/cemanager.cpp \
        ../../model/CanvasElements/canvaselement.cpp \
        ../../model/DrawingClassifiers/idrawingclassifier.cpp \
        ../../model/DrawingClassifiers/neuralclassifier.cpp \
        ../../model/DrawingClassifiers/simpleclassifier.cpp \
        ../../model/canvasmanipulation.cpp \
        ../../model/connector.cpp \
        ../../model/logger.cpp \
        ../../model/model.cpp \
        ../../model/modelStateOp.cpp \
        ../../model/modelstatemanager.cpp \
        ../../model/identityDir/connectiondata.cpp \
        ../../model/identityDir/identifiable.cpp \
        ../../model/identityDir/qobjectident.cpp \
        ../../model/myqline.cpp \
        ../../model/plug.cpp \
        ../../model/userinputmodel.cpp \
        ../../mysharedptr.cpp \
        ../../network/network.cpp \
        #painter_main/main.cpp \
        ../../persistence/persistence.cpp \
        ../../shape_drawing/drawing.cpp \
        ../../shape_drawing/drawing_factory.cpp \
        ../../shape_drawing/gesture.cpp \
        ../../qrandomgenerator.cpp
HEADERS += \
        ../../common.hpp \
        ../../functions.hpp \
        ../../model/model.h \
        ../../model/CanvasElements/canvaselement.h \
        ../../model/CanvasElements/canvaselementbase.h \
        ../../model/CanvasElements/canvaselementtypes.h \
        ../../model/CanvasElements/cemanager.h \
        ../../model/DrawingClassifiers/idrawingclassifier.h \
        ../../model/DrawingClassifiers/neuralclassifier.h \
        ../../model/DrawingClassifiers/simpleclassifier.h \
        ../../model/canvasmanipulation.h \
        ../../model/connector.h \
        ../../model/dirDim.hpp \
        ../../model/idrawingcontainer.h \
        ../../model/initializer.hpp \
        ../../model/logger.h \
        ../../model/model.h \
        ../../model/modelStateOp.h \
        ../../model/modelStateOp_impl.h \
        ../../model/modelstatemanager.h \
        ../../model/identityDir/connectiondata.h \
        ../../model/identityDir/identifiable.h \
        ../../model/identityDir/qobjectident.h \
        ../../model/myqline.h \
        ../../model/myweakptr.h \
        ../../model/myweakptr_impl.h \
        ../../model/plug.h \
        ../../model/target.h \
        ../../model/userinputmodel.h \
        ../../mysharedptr.h \
        ../../network/network.h \
        ../../persistence/persistence.h \
        ../../persistence/streamoperators.h \
        ../../persistence/streamoperators_impl.h \
        ../../shape_drawing/drawing_factory.h \
        ../../shape_drawing/gesture.h \
        ../../shape_drawing/drawing.h \
        ../../qrandomgenerator.h 

INCLUDEPATH += ../../

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
