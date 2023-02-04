#pragma once
#define TESTING
#include "model/model.h"

//#include <QtTest/QtTest>
namespace Unittest{
class ModelUnittest: public QObject
{
	Q_OBJECT
  Model* model = nullptr;
public:
	ModelUnittest();
private slots:
	// a testcase is a group of testfunctions

    // init testcase
    void initTestCase();
    // cleanup testcase
    void cleanupTestCase();
	// init testfunction
    void init();
    // cleanup testfunction
    void cleanup();
    
    //
    //testfunctions:
    //
    void ctorTest1();
    void ctorTest2();
    void ctorTest3();
    
    void dtorTest1();
    
    void ddamTest1();
    void ddamTest2();
    void ddamTest3();
    void ddamTest4();
    
    void gldTest();
    
    void indTest1();
    void indTest2();
    
    void prrldTest1();
    void prrldTest2();
    void prrldTest3();
    void prrldTest4();
    
};
}
