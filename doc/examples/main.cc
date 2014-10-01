#include <iostream>

void RunMTAPI_C();
void RunMTAPI_CPP();
void RunDataflowLinear();
void RunDataflowNonLinear();
void RunSTLForEach();
void RunForEach();
void RunInvoke();
void RunSorting();
void RunReduce();
void RunCounting();
void RunScan();
void RunObjectPoolExamples();
void RunStackExamples();
void RunQueueExamples();

/**
 * Runs all examples and tests their correctness.
 */
int main() {
  std::cout << "Running examples ..." << std::endl;

  std::cout << "RunMTAPI_C() ..." << std::endl;
  RunMTAPI_C();
  std::cout << "RunMTAPI_C() ... done" << std::endl;

  std::cout << "RunMTAPI_CPP() ..." << std::endl;
  RunMTAPI_CPP();
  std::cout << "RunMTAPI_CPP() ... done" << std::endl;

  std::cout << "RunDataflowLinear() ..." << std::endl;
  RunDataflowLinear();
  std::cout << "RunDataflowLinear() ... done" << std::endl;

  std::cout << "RunDataflowNonLinear() ..." << std::endl;
  RunDataflowNonLinear();
  std::cout << "RunDataflowNonLinear() ... done" << std::endl;

  std::cout << "RunSTLForEach() ..." << std::endl;
  RunSTLForEach();
  std::cout << "RunSTLForEach() ... done" << std::endl;

  std::cout << "RunForEach() ..." << std::endl;
  RunForEach();
  std::cout << "RunForEach() ... done" << std::endl;

  std::cout << "RunInvoke() ..." << std::endl;
  RunInvoke();
  std::cout << "RunInvoke() ... done" << std::endl;
  
  std::cout << "RunSorting() ... " << std::endl;
  RunSorting();
  std::cout << "RunSorting() ... done" << std::endl;

  std::cout << "RunReduce() ... " << std::endl;
  RunReduce();
  std::cout << "RunReduce() ... done" << std::endl;

  std::cout << "RunCounting() ..." << std::endl;
  RunCounting();
  std::cout << "RunCounting() ... done" << std::endl;
  /*
  std::cout << "RunScan() ..." << std::endl;
  RunScan();
  std::cout << "RunScan() ... done" << std::endl;
  */
  std::cout << "RunObjectPoolExamples() ..." << std::endl;
  RunObjectPoolExamples();
  std::cout << "RunObjectPoolExamples() ... done" << std::endl;

  std::cout << "RunStackExamples() ..." << std::endl;
  RunStackExamples();
  std::cout << "RunStackExamples() ... done" << std::endl;

  std::cout << "RunQueueExamples() ..." << std::endl;
  RunQueueExamples();
  std::cout << "RunQueueExamples() ... done" << std::endl;

  std::cout << "Running examples ... done" << std::endl;
}
