#include <iostream>
#include <string>
#include <sstream>
#include "OrderCache.h"
#include "gtest/gtest.h"


TEST(OrderCacheTest, AddOrder) {
    OrderCache cache;
    cache.addOrder(Order{"OrdId1", "SecId1", "Buy", 1000, "User1", "CompanyA"});
    cache.addOrder(Order{"OrdId2", "SecId2", "Sell", 3000, "User2", "CompanyB"});
    cache.addOrder(Order{"OrdId3", "SecId1", "Sell", 500, "User3", "CompanyA"});
    cache.addOrder(Order{"OrdId4", "SecId2", "Buy", 600, "User4", "CompanyC"});
    cache.addOrder(Order{"OrdId5", "SecId2", "Buy", 100, "User5", "CompanyB"});
    cache.addOrder(Order{"OrdId6", "SecId3", "Buy", 1000, "User6", "CompanyD"});
    cache.addOrder(Order{"OrdId7", "SecId2", "Buy", 2000, "User7", "CompanyE"});
    cache.addOrder(Order{"OrdId8", "SecId2", "Sell", 5000, "User8", "CompanyE"});

    std::vector<Order> allOrders = cache.getAllOrders();
    ASSERT_EQ(allOrders.size(), 8);
}

TEST(OrderCacheTest, GetAllOrders) {
    OrderCache cache;
    cache.addOrder(Order{"1", "SecId1", "BUY", 100, "User1", "Company1"});
    cache.addOrder(Order{"2", "SecId1", "SELL", 100, "User2", "Company1"});

    std::vector<Order> allOrders = cache.getAllOrders();

    ASSERT_EQ(allOrders.size(), 2);
}

TEST(OrderCacheTest, CancelOrder) {
    OrderCache cache;
    cache.addOrder(Order{"1", "SecId1", "BUY", 100, "User1", "Company1"});
    cache.addOrder(Order{"2", "SecId1", "SELL", 100, "User2", "Company1"});

    std::vector<Order> allOrders = cache.getAllOrders();
    ASSERT_EQ(allOrders.size(), 2);

    // Cancel order 2
    cache.cancelOrder("2");
    allOrders = cache.getAllOrders();
    ASSERT_EQ(allOrders.size(), 1);
    // the map structure doesn't keep same order than the vector structure
    #ifndef MAP_OPTIMIZATION
    ASSERT_EQ(allOrders[0].orderId(), "1");
    #endif

    // Cancel order 1
    cache.cancelOrder("1");
    allOrders = cache.getAllOrders();
    ASSERT_EQ(allOrders.size(), 0);
}

TEST(OrderCacheTest, CancelOrdersForUser) {
    OrderCache cache;
    cache.addOrder(Order{"1", "SecId1", "BUY", 100, "User1", "Company1"});
    cache.addOrder(Order{"2", "SecId1", "SELL", 100, "User2", "Company1"});

    std::vector<Order> allOrders = cache.getAllOrders();
    ASSERT_EQ(allOrders.size(), 2);

    // Cancel order for User2
    cache.cancelOrdersForUser("User2");
    allOrders = cache.getAllOrders();
    ASSERT_EQ(allOrders.size(), 1);
    #ifndef MAP_OPTIMIZATION
    ASSERT_EQ(allOrders[0].orderId(), "1");
    #endif

    // Cancel order for User1
    cache.cancelOrdersForUser("User1");
    allOrders = cache.getAllOrders();
    ASSERT_EQ(allOrders.size(), 0);
}

TEST(OrderCacheTest, CancelOrdersForSecIdWithMinimumQty) {
    OrderCache cache;
    cache.addOrder(Order{"1", "SecId1", "BUY", 200, "User1", "Company1"});
    cache.addOrder(Order{"2", "SecId1", "SELL", 200, "User2", "Company1"});
    cache.addOrder(Order{"3", "SecId1", "BUY", 100, "User1", "Company1"});

    cache.cancelOrdersForSecIdWithMinimumQty("SecId1", 300);
    std::vector<Order> allOrders = cache.getAllOrders();
    ASSERT_EQ(allOrders.size(), 3);

    cache.cancelOrdersForSecIdWithMinimumQty("SecId1", 200);
    allOrders = cache.getAllOrders();
    ASSERT_EQ(allOrders.size(), 1);

    cache.cancelOrdersForSecIdWithMinimumQty("SecId1", 100);
    allOrders = cache.getAllOrders();
    ASSERT_EQ(allOrders.size(), 0);
}

TEST(OrderCacheTest, GetMatchingSizeForSecurityTest1) {
    OrderCache cache;

    cache.addOrder(Order{"OrdId1", "SecId1", "Buy", 1000, "User1", "CompanyA"});
    cache.addOrder(Order{"OrdId2", "SecId2", "Sell", 3000, "User2", "CompanyB"});
    cache.addOrder(Order{"OrdId3", "SecId1", "Sell", 500, "User3", "CompanyA"});
    cache.addOrder(Order{"OrdId4", "SecId2", "Buy", 600, "User4", "CompanyC"});
    cache.addOrder(Order{"OrdId5", "SecId2", "Buy", 100, "User5", "CompanyB"});
    cache.addOrder(Order{"OrdId6", "SecId3", "Buy", 1000, "User6", "CompanyD"});
    cache.addOrder(Order{"OrdId7", "SecId2", "Buy", 2000, "User7", "CompanyE"});
    cache.addOrder(Order{"OrdId8", "SecId2", "Sell", 5000, "User8", "CompanyE"});

    unsigned int matchingSize = cache.getMatchingSizeForSecurity("SecId1");
    ASSERT_EQ(matchingSize, 0);

    matchingSize = cache.getMatchingSizeForSecurity("SecId2");
    ASSERT_EQ(matchingSize, 2700);

    matchingSize = cache.getMatchingSizeForSecurity("SecId3");
    ASSERT_EQ(matchingSize, 0);
}

TEST(OrderCacheTest, GetMatchingSizeForSecurityTest2) {
    OrderCache cache;
    cache.addOrder(Order{"OrdId1", "SecId1", "Sell", 100, "User10", "Company2"});
    cache.addOrder(Order{"OrdId2", "SecId3", "Sell", 200, "User8", "Company2"});
    cache.addOrder(Order{"OrdId3", "SecId1", "Buy", 300, "User13", "Company2"});
    cache.addOrder(Order{"OrdId4", "SecId2", "Sell", 400, "User12", "Company2"});
    cache.addOrder(Order{"OrdId5", "SecId3", "Sell", 500, "User7", "Company2"});
    cache.addOrder(Order{"OrdId6", "SecId3", "Buy", 600, "User3", "Company1"});
    cache.addOrder(Order{"OrdId7", "SecId1", "Sell", 700, "User10", "Company2"});
    cache.addOrder(Order{"OrdId8", "SecId1", "Sell", 800, "User2", "Company1"});
    cache.addOrder(Order{"OrdId9", "SecId2", "Buy", 900, "User6", "Company2"});
    cache.addOrder(Order{"OrdId10", "SecId2", "Sell", 1000, "User5", "Company1"});
    cache.addOrder(Order{"OrdId11", "SecId1", "Sell", 1100, "User13", "Company2"});
    cache.addOrder(Order{"OrdId12", "SecId2", "Buy", 1200, "User9", "Company2"});
    cache.addOrder(Order{"OrdId13", "SecId1", "Sell", 1300, "User1", "Company"});

    unsigned int matchingSize = cache.getMatchingSizeForSecurity("SecId1");
    ASSERT_EQ(matchingSize, 300);

    matchingSize = cache.getMatchingSizeForSecurity("SecId2");
    ASSERT_EQ(matchingSize, 1000);

    matchingSize = cache.getMatchingSizeForSecurity("SecId3");
    ASSERT_EQ(matchingSize, 600);
}

TEST(OrderCacheTest, GetMatchingSizeForSecurityTest3) {
    OrderCache cache;
    cache.addOrder(Order{"OrdId1", "SecId3", "Sell", 100, "User1", "Company1"});
    cache.addOrder(Order{"OrdId2", "SecId3", "Sell", 200, "User3", "Company2"});
    cache.addOrder(Order{"OrdId3", "SecId1", "Buy", 300, "User2", "Company1"});
    cache.addOrder(Order{"OrdId4", "SecId3", "Sell", 400, "User5", "Company2"});
    cache.addOrder(Order{"OrdId5", "SecId2", "Sell", 500, "User2", "Company1"});
    cache.addOrder(Order{"OrdId6", "SecId2", "Buy", 600, "User3", "Company2"});
    cache.addOrder(Order{"OrdId7", "SecId2", "Sell", 700, "User1", "Company1"});
    cache.addOrder(Order{"OrdId8", "SecId1", "Sell", 800, "User2", "Company1"});
    cache.addOrder(Order{"OrdId9", "SecId1", "Buy", 900, "User5", "Company2"});
    cache.addOrder(Order{"OrdId10", "SecId1", "Sell", 1000, "User1", "Company1"});
    cache.addOrder(Order{"OrdId11", "SecId2", "Sell", 1100, "User6", "Company2"});

    unsigned int matchingSize = cache.getMatchingSizeForSecurity("SecId1");
    ASSERT_EQ(matchingSize, 900);

    matchingSize = cache.getMatchingSizeForSecurity("SecId2");
    ASSERT_EQ(matchingSize, 600);

    matchingSize = cache.getMatchingSizeForSecurity("SecId3");
    ASSERT_EQ(matchingSize, 0);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}