This is a test I received in C++
I want to share my resolution only for portfolio reasons
The test description can be read in TEST-README.txt file
I won't share who sent me the test

I did FOUR versions of the test to show performance benchmark comparison:
1) non-optimized
2) optimized only with MAP_OPTIMIZATION
3) optimized only with ERASE_OPTIMIZATION
4) optimized with both

The optimizers are defined as headers on the OrderCache.h
#define ERASE_OPTIMIZATION // about 15% speed improvement, especially for big amounts of data
#define MAP_OPTIMIZATION // about 50% speed improvement, especially for large amounts of data
They are commented on so the compiler will decide on the optimization parameters

Files I didn't touch?
main.cpp
TEST-README.txt
TEST-TESTING.txt

Files I modified (the only modifications are shown below):
OrderCache.h
  Added the member "void setQty(unsigned int qty)"
  Added all the private members of the class OrderCache
OrderCache.cpp
  Implemented all the interface methods

Files added by me:
build-all # script to build all versions of the assertion tests and all versions of the performance tests
MatchingSizeHelpers.cpp # helps implement the interface
OrderCachePerformanceTests.cpp # to run the performance test
OrderCachePerformanceTestsHelpers.cpp
OrderCachePerformanceTestsHelpers.h
README.txt
run-all-assertion-tests
run-all-performance-tests

