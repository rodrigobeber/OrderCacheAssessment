#include "OrderCache.h"
#include <algorithm>
#include <memory>

std::pair<std::unique_ptr<std::vector<Order>>, std::unique_ptr<std::vector<Order>>> OrderCache::getBookBySecId(const std::string& securityId) const {
  auto buyOrders = std::make_unique<std::vector<Order>>();
  auto sellOrders = std::make_unique<std::vector<Order>>();

  #ifdef MAP_OPTIMIZATION
  auto it = allOrders.find(securityId);
  if (it != allOrders.end()) {
    for (const Order& order: it->second) {
  #else
  for (const Order& order: allOrders) {
  #endif
      if (order.securityId() == securityId) {
        // split the orders in their respective vectors
        if (order.side() == "Buy") {
          buyOrders->push_back(order);
        } else {
          sellOrders->push_back(order);
        }
      }
    }
  #ifdef MAP_OPTIMIZATION
  }
  #endif
  return std::make_pair(std::move(buyOrders), std::move(sellOrders));
}

std::vector<Order>::iterator OrderCache::findFirstMatchingOrder(std::vector<Order>::iterator start, const std::vector<Order>::iterator end, const Order& order) const {
    return std::find_if(start, end, [&order](const Order& other) {
        #ifdef ERASE_OPTIMIZATION
        return order.company() != other.company();
        #else
        return order.company() != other.company() && other.qty() > 0;
        #endif
    });
}

unsigned int OrderCache::matchOrders(Order& buyOrder, Order& sellOrder) const {
  unsigned int mSize = 0;
  // obs.: these orders are a copy, so changing their 'qty' won't affect 'allOrders'
  if (buyOrder.qty() >= sellOrder.qty()) {
      mSize = sellOrder.qty();
      buyOrder.setQty(buyOrder.qty() - sellOrder.qty());
      sellOrder.setQty(0);
  } else {
      mSize = buyOrder.qty();
      sellOrder.setQty(sellOrder.qty() - buyOrder.qty());
      buyOrder.setQty(0);
  }
  // return the matching size for these two orders
  return mSize;
}

unsigned int OrderCache::tryMatch(Order& buyOrder, std::unique_ptr<std::vector<Order>>& sellOrders) const {
  unsigned int mSize = 0;
  auto sellOrderIt = sellOrders->begin();
  while (buyOrder.qty() > 0) {
    sellOrderIt = findFirstMatchingOrder(sellOrderIt, sellOrders->end(), buyOrder);
    if (sellOrderIt == sellOrders->end()) {
      break;
    }
    // consume their quantities and increment the matching size
    mSize += matchOrders(buyOrder, *sellOrderIt);

    #ifdef ERASE_OPTIMIZATION
    // remove the sell order if it has been entirely consumed
    if ((*sellOrderIt).qty() == 0) {
      sellOrderIt = sellOrders->erase(sellOrderIt);
    }
    #endif
  }
  // return the matching size for a 'Buy' order
  return mSize;
}