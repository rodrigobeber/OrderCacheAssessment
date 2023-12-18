#include <string>
#include "OrderCache.h"


using params_tuple = std::tuple<unsigned int, unsigned int, unsigned int>;

class OrderCachePerformanceTestsHelpers {
public:
    static unsigned int getRandom(unsigned int n);
    static std::string makeSecId(int i);
    static OrderCache* generateOrders(unsigned int numOfOrders, unsigned int numOfSecurities, unsigned int numOfCompanies);
    static void runTest(unsigned int testId, unsigned int numOfOrders, unsigned int numOfSecurities, unsigned int numOfCompanies);
};

