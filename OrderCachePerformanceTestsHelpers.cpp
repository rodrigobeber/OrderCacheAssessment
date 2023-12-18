#include <iostream>
#include <string>
#include <thread>
#include <random>
#include <chrono>
#include <fstream>
#include <sstream>
#include "OrderCachePerformanceTestsHelpers.h"

unsigned int OrderCachePerformanceTestsHelpers::getRandom(unsigned int n) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<unsigned int> dis(1, n);

    return dis(gen);
}

std::string OrderCachePerformanceTestsHelpers::makeSecId(int i) {
    return "secId" + std::to_string(i);
}

OrderCache* OrderCachePerformanceTestsHelpers::generateOrders(unsigned int numOfOrders, unsigned int numOfSecurities, unsigned int numOfCompanies) {
    OrderCache* cache = new OrderCache();
    for (int i = 1; i <= numOfOrders; i++) {
        const std::string secId = OrderCachePerformanceTestsHelpers::makeSecId(OrderCachePerformanceTestsHelpers::getRandom(numOfSecurities));
        const std::string type = OrderCachePerformanceTestsHelpers::getRandom(2) == 1 ? "Buy" : "Sell";
        const unsigned int qty = OrderCachePerformanceTestsHelpers::getRandom(50) * 100;
        const std::string companyName = "Company" + std::to_string(OrderCachePerformanceTestsHelpers::getRandom(numOfCompanies));
        cache->addOrder(Order{"OrdId" + std::to_string(i), secId, type, qty, "UserXYZ", companyName});
    }
    return cache;
}

void OrderCachePerformanceTestsHelpers::runTest(unsigned int testId, unsigned int numOfOrders, unsigned int numOfSecurities, unsigned int numOfCompanies) {

    OrderCache* cache = OrderCachePerformanceTestsHelpers::generateOrders(numOfOrders, numOfSecurities, numOfCompanies);

    auto startMatch = std::chrono::high_resolution_clock::now();
    for (int i = 1; i <= numOfSecurities; i++) {
        const std::string secId = OrderCachePerformanceTestsHelpers::makeSecId(i);
        unsigned int mSize = cache->getMatchingSizeForSecurity(secId);
    }
    auto endMatch = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> durationMatch = endMatch - startMatch;

    free(cache);

    std::cout << "Test " << testId << " MATCHING execution time: " << durationMatch.count() << " milliseconds" << std::endl;
}
