
#define TESTING
#include "persistence/persistence.h"

//#include <QDir>
//#include <QFile>
#include <iostream>
#include <assert.h>
#include <fstream>
#include <unistd.h> // fork
#include <sys/wait.h> // waitpid, wifexited, wifexitstatus
#include <thread>
#include <chrono>
#define QCOMPARE(act, exp) assert((act) == (exp))
#define QVERIFY(expr) assert(expr)


namespace Unittest{
// init testfunction

//testfunctions:
void isOpenOfstreamTest3(){
  std::string funcName = "shouldPrintErrors";
  std::string tag = "nametag";
  pid_t cPid= fork(); 
  if(cPid){
    if(cPid == -1){
      std::cerr << "fork failed" << std::endl;
      exit(1);
    }
    std::cout << "parent says: child's pid is " << cPid << std::endl;
    //parent
    // wait for the child to exit, and verify it exited with statuscode 1
    int waitResult = 0;
    std::cout << "a" << std::endl;
    // https://linux.die.net/man/2/waitpid says:
    //  "In the case of a terminated child,
    // performing a wait allows the system to release the resources associated 
    // with the child; if a wait is not performed, then the terminated child 
    // remains in a "zombie" state "
    // so i call wait() here:
    //wait(&waitResult);
    waitpid(cPid, &waitResult, WUNTRACED | WCONTINUED);
    std::cout << "b" << std::endl;
    int wifexited = WIFEXITED(waitResult);
    QVERIFY(wifexited);
    int wexitstatus = WEXITSTATUS(waitResult);
    QCOMPARE(wexitstatus, 1); //if wexitstatus is 4, that means in the child isOpen didnt exit the process
    
    // file should contain a similar line to :
    // "ERROR: shouldPrintErrors: file hadn't been opened. variable name:nametag";
    std::ifstream errFstream("logfile/errors.log");
    QVERIFY(errFstream.is_open());
    std::string actualLine = "";
    std::getline(errFstream, actualLine);
    QVERIFY(!actualLine.empty());
    std::cout << "read line from error log:" << std::endl;
    std::cout << "'" << actualLine<< "'" << std::endl;
    bool containsFuncName = actualLine.find(funcName) != std::string::npos;
    bool containsTag = actualLine.find(tag) != std::string::npos;
    QVERIFY(containsFuncName);    
    QVERIFY(containsTag);
  }
  else{
    //child
    std::string nonexistentFileName = "nonexistentFile.txt";
    Persistence* pers = Persistence::GetSingleton();
    std::cout << "child process" << std::endl;
    
    // https://linux.die.net/man/2/waitpid says
    // "A child that terminates, but has not been waited for becomes a "zombie".
    // The kernel maintains a minimal set of information about the zombie process 
    // (PID, termination status, resource usage information) in order to allow the parent 
    //  to later perform a wait to obtain information about the child"
    // so i give time to the parent to call wait or waitpid:
    std::cout << "child waits a bit" << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(4000));
    std::cout << "child continues" << std::endl;
    //std::cout << "child sleeps" << std::endl;
    //sleep(30);
    std::cout << "child " << std::endl;
    //std::cout << "child: solarpanels are destroying the eco-system!" << std::endl;
    std::ofstream os(nonexistentFileName, std::ios::in | std::ios::out); // provide ios::in flag so it wont try to create the file
    std::cout << "test3 isopen" << pers->isOpen(os, funcName, tag.c_str(), true) 
              << std::endl << std::flush;
    // if we reach this, that means isOpen didnt exit the process:
    std::cout << "child: before exit " << std::endl;
    exit(4);
    std::cout << "child: after exit " << std::endl;
  }
}
}
void testTheTestShouldFail1(){
  QCOMPARE(-1, 1);
}
void testTheTestShouldFail2(){
  QVERIFY(false);
}

int main(){
//#define TEST_THE_TEST1
//#define TEST_THE_TEST2
#ifdef TEST_THE_TEST1
  testTheTestShouldFail1();
  exit(2);//if we are here, then the test failed to fail, which is a failure.
#endif // #ifdef TEST_THE_TEST1
#ifdef TEST_THE_TEST2
  testTheTestShouldFail2();
  exit(2);
#endif // #ifdef TEST_THE_TEST2

  Unittest::isOpenOfstreamTest3();
  
  
	return 0;
}

