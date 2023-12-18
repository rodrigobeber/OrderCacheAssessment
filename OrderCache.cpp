// Your implementation of the OrderCache...
#include "OrderCache.h"
#include <algorithm>

void OrderCache::addOrder(Order order) {
  std::lock_guard<std::mutex> lock(ordersMutex);
  #ifdef MAP_OPTIMIZATION
  auto result = allOrders.emplace(order.securityId(), std::vector<Order>{});
  result.first->second.push_back(order);
  #else
  allOrders.push_back(order);
  #endif
}

void OrderCache::cancelOrder(const std::string& orderId) {
  std::lock_guard<std::mutex> lock(ordersMutex);
  #ifdef MAP_OPTIMIZATION
  for (auto& pair : allOrders) {
    std::vector<Order>& orders = pair.second;
    auto it = std::find_if(orders.begin(), orders.end(), [&orderId](const Order& order) { return order.orderId() == orderId; });
    if (it != orders.end()) {
      orders.erase(it);
      // if no more orders for the security, erase the map entry
      if (orders.empty()) {
          allOrders.erase(pair.first);
      }
      // since orderId is unique, break
      break;
    }
  }
  #else
  //allOrders.erase(std::remove_if(allOrders.begin(), allOrders.end(),
  //    [&orderId](const Order& order) { return order.orderId() == orderId; }
  //  ), allOrders.end());

  auto it = std::find_if(allOrders.begin(), allOrders.end(),
      [&orderId](const Order& order) { return order.orderId() == orderId; });


  #endif
}

#ifdef MAP_OPTIMIZATION
void OrderCache::removeOrders(std::function<bool(const Order&)> condition) {
  // for each security
  auto it = allOrders.begin();
  while (it != allOrders.end()) {
    std::vector<Order>& orders = it->second;
    bool entryErased = false;
    // for each security order
    auto vecIt = orders.begin();
    while (vecIt != orders.end()) {
      if (condition(*vecIt)) {
        vecIt = orders.erase(vecIt);
        if (orders.empty()) {
          it = allOrders.erase(it);
          entryErased = true;
          break;
        }
      } else {
        vecIt = std::next(vecIt);
      }
    }
    if (!entryErased) {
      it = std::next(it);
    }
  }
}
#endif

void OrderCache::cancelOrdersForUser(const std::string& user) {
  std::lock_guard<std::mutex> lock(ordersMutex);
  #ifdef MAP_OPTIMIZATION
  removeOrders([&user](const Order& order) { return order.user() == user; });
  #else
  allOrders.erase(std::remove_if(allOrders.begin(), allOrders.end(),
      [&user](const Order& order) { return order.user() == user; }
    ), allOrders.end());
  #endif
}

void OrderCache::cancelOrdersForSecIdWithMinimumQty(const std::string& securityId, unsigned int minQty) {
  std::lock_guard<std::mutex> lock(ordersMutex);
  #ifdef MAP_OPTIMIZATION
  removeOrders([&securityId, &minQty](const Order& order) { return order.securityId() == securityId && order.qty() >= minQty; });
  #else
  allOrders.erase(std::remove_if(allOrders.begin(), allOrders.end(),
      [&securityId, &minQty](const Order& order) { return order.securityId() == securityId && order.qty() >= minQty; }
    ), allOrders.end());
  #endif
}

unsigned int OrderCache::getMatchingSizeForSecurity(const std::string& securityId) {
  auto book = getBookBySecId(securityId);
  unsigned int mSize = 0;
  // for each order of side 'Buy'
  for (Order& buyOrder: *(book.first)) {
    // try to match with order of side 'Sell'
    mSize += tryMatch(buyOrder, book.second);
  }
  return mSize;
}

std::vector<Order> OrderCache::getAllOrders() const {
  #ifdef MAP_OPTIMIZATION
  // build a single vector from the map
  std::vector<Order> vecAllOrders{};
  for (auto& pair : allOrders) {
    std::vector<Order> orders = pair.second;
    vecAllOrders.insert(vecAllOrders.end(), orders.begin(), orders.end());
  }
  return vecAllOrders;
  #else
  return allOrders;
  #endif
}

