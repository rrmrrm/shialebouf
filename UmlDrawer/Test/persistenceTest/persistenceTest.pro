QT += widgets testlib

CONFIG += c++17

SOURCES += \
    #modelunittest.cpp \
    #viewmock.cpp \
    persistenceTest.cpp \
    ../../model/CanvasElements/canvaselementbase.cpp \
    ../../model/CanvasElements/cemanager.cpp \
    ../../model/CanvasElements/canvaselement.cpp \
    ../../model/DrawingClassifiers/idrawingclassifier.cpp \
    ../../model/DrawingClassifiers/neuralclassifier.cpp \
    ../../model/DrawingClassifiers/simpleclassifier.cpp \
    ../../model/canvasmanipulation.cpp \
    ../../model/connector.cpp \
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
    ../../persistence/logger.cpp \
    ../../shape_drawing/drawing.cpp \
    ../../shape_drawing/drawing_factory.cpp \
    ../../shape_drawing/gesture.cpp \
    ../../qrandomgenerator.cpp #\ # uncomment this backslash for magical compiler warnings about circular dependency of 'HEADERS' and of '+='
HEADERS += \
    #modelunittest.h \
    #viewmock.h \
    #whiteBox.h \
    persistenceTest.h \
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
    ../../persistence/logger.h \
    ../../persistence/streamoperators.h \
    ../../persistence/streamoperators_impl.h \
    ../../shape_drawing/drawing_factory.h \
    ../../shape_drawing/gesture.h \
    ../../shape_drawing/drawing.h \
    ../../qrandomgenerator.h 
    
INCLUDEPATH += ../../

# install
INSTALLS += target
