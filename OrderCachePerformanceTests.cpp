#include <iostream>
#include <string>
#include <thread>
#include <random>
#include <chrono>
#include "OrderCache.h"
#include "OrderCachePerformanceTestsHelpers.h"

int main() {

    #if defined MAP_OPTIMIZATION && defined ERASE_OPTIMIZATION
        std::cout << "MAP AND ERASE OPTIMIZATIONS:" << std::endl;
    #elif defined MAP_OPTIMIZATION
        std::cout << "MAP OPTIMIZATION ONLY:" << std::endl;
    #elif defined ERASE_OPTIMIZATION
        std::cout << "ERASE OPTIMIZATION ONLY:" << std::endl;
    #else
        std::cout << "NO OPTIMIZATIONS:" << std::endl;
    #endif


    std::vector<std::tuple<unsigned int, unsigned int, unsigned int>> params = {
        {5'000, 20, 2'000},
        {10'000, 20, 8'000},
        {50'000, 100, 45'000},
        {100'000, 100, 80'000},
        {1'000'000, 200, 800'000}
    };

    std::vector<std::thread> threads;
    for (size_t i = 0; i < params.size(); ++i) {
        threads.emplace_back(OrderCachePerformanceTestsHelpers::runTest, i + 1, std::get<0>(params[i]), std::get<1>(params[i]), std::get<2>(params[i]));
    }

    for (auto& thread : threads) {
        thread.join();
    }

    std::cout << std::endl;

    return 0;
}