#pragma once

#include <string>
#include <vector>
#include <mutex>
#include <unordered_map>
#include <memory>
#include <functional>

//#define ERASE_OPTIMIZATION // about 15% of speed improvement, specially for big amounts of data
//#define MAP_OPTIMIZATION // about 50% of speed improvement, specially for big amounts of data

class Order
{

 public:

  // do not alter signature of this constructor
  Order(
      const std::string& ordId,
      const std::string& secId,
      const std::string& side,
      const unsigned int qty,
      const std::string& user,
      const std::string& company)
      : m_orderId(ordId),
        m_securityId(secId),
        m_side(side),
        m_qty(qty),
        m_user(user),
        m_company(company) { }

  // do not alter these accessor methods
  std::string orderId() const    { return m_orderId; }
  std::string securityId() const { return m_securityId; }
  std::string side() const       { return m_side; }
  std::string user() const       { return m_user; }
  std::string company() const    { return m_company; }
  unsigned int qty() const       { return m_qty; }
  void setQty(unsigned int qty) {
    m_qty = qty;
  }

 private:

  // use the below to hold the order data
  // do not remove the these member variables
  std::string m_orderId;     // unique order id
  std::string m_securityId;  // security identifier
  std::string m_side;        // side of the order, eg Buy or Sell
  unsigned int m_qty;        // qty for this order
  std::string m_user;        // user name who owns this order
  std::string m_company;     // company for user

};

class OrderCacheInterface
{

 public:

  // implement the 6 methods below, do not alter signatures

  // add order to the cache
  virtual void addOrder(Order order) = 0;

  // remove order with this unique order id from the cache
  virtual void cancelOrder(const std::string& securityId) = 0;

  // remove all orders in the cache for this user
  virtual void cancelOrdersForUser(const std::string& user) = 0;

  // remove all orders in the cache for this security with qty >= minQty
  virtual void cancelOrdersForSecIdWithMinimumQty(const std::string& securityId, unsigned int minQty) = 0;

  // return the total qty that can match for the security id
  virtual unsigned int getMatchingSizeForSecurity(const std::string& securityId) = 0;

  // return all orders in cache in a vector
  virtual std::vector<Order> getAllOrders() const = 0;

};

// Provide an implementation for the OrderCacheInterface interface class.
// Your implementation class should hold all relevant data structures you think
// are needed.
class OrderCache : public OrderCacheInterface
{
  // protect 'add' and 'cancel' operations
  std::mutex ordersMutex;

  // obs.: if the most frequent operations were matching orders, and if I could change the signature of the methods,
  // then I would prefer grouping the orders with the same security id in a std::unordered_map<std::string, vector<Order>>
  // (using securityId as key) instead of std::vector
  #ifdef MAP_OPTIMIZATION
  std::unordered_map<std::string, std::vector<Order>> allOrders;
  #else
  std::vector<Order> allOrders;
  #endif

  // all private methods bellow are used only by 'getMatchingSizeForSecurity'

  // return the order's book of a security Id (one vector for 'Buy' orders and the other for 'Sell' orders)
  // the purpose is to optimize the matching algorithm using these structures separately
  std::pair<std::unique_ptr<std::vector<Order>>, std::unique_ptr<std::vector<Order>>> getBookBySecId(const std::string& securityId) const;

  // calculates the matching size for a given order and 'consumes' the orders updating their qty
  unsigned int tryMatch(Order& buyOrder, std::unique_ptr<std::vector<Order>>& sellOrders) const;

  // find first match for the given order
  std::vector<Order>::iterator findFirstMatchingOrder(std::vector<Order>::iterator start, const std::vector<Order>::iterator end, const Order& order) const;

  // update orders quantities and returns the matching size between these two orders
  unsigned int matchOrders(Order& buyOrder, Order& sellOrder) const;

  #ifdef MAP_OPTIMIZATION
  // remove orders given a predicate
  void removeOrders(std::function<bool(const Order&)> condition);
  #endif

 public:

  void addOrder(Order order) override;

  void cancelOrder(const std::string& securityId) override;

  void cancelOrdersForUser(const std::string& user) override;

  void cancelOrdersForSecIdWithMinimumQty(const std::string& securityId, unsigned int minQty) override;

  unsigned int getMatchingSizeForSecurity(const std::string& securityId) override;

  std::vector<Order> getAllOrders() const override;
};