/**
 * @file goods.h
 * @author your name (you@domain.com)
 * @brief This header file includes goods class
 * @version 0.1
 * @date 2023-01-21
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <string>

namespace GoodsTrack {
template <typename T = double>
class Goods {
 public:
  Goods() = default;

  /**
   * @brief Constructor of Goods
   *
   * @param id shows the id of the Goods object in the database
   * @param name shows the name of the goods
   * @param amount shows the amount of the goods
   * @param price shows the price of the goods
   * @param total shows the total price of the goods
   * @param currency shows the currency of the goods
   */
  Goods( int id, std::string name, int amount, T price, T total, std::string currency )
      : m_Id { id }, m_Name { std::move( name ) }, m_Amount { amount }, m_Price { price }, m_Total { total }, m_Currency { std::move( currency ) } {}

  /**
   * @brief set the id of the goods
   * @param id id of the goods
   */
  void setId( int id ) { m_Id = id; }
  /**
   * @brief get the id of the goods
   * @return the value of the id of the goods
   */
  int getId() const { return m_Id; }

  /**
   * @brief set the name of the goods
   * @param name name of the goods
   */
  void setName( std::string_view name ) { m_Name = name.data(); }
  /**
   * @brief Get the name of the goods
   * @return std::string name of the goods
   */
  std::string getName() const { return m_Name; }

  /**
   * @brief Set the Amount of the goods
   * @param amount amount of the goods
   */

  void setAmount( int amount ) { m_Amount = amount; }

  /**
   * @brief Get the Amount of the goods
   * @return double amount of the goods
   */
  int getAmount() const { return m_Amount; }

  /**
   * @brief set the price of the goods
   * @param price the price of the goods
   */

  void setPrice( T price ) { m_Price = price; }
  /**
   * @brief get the price of the goods
   * @return the price of the goods
   */

  T getPrice() const { return m_Price; }
  /**
   * @brief
   * @param total
   */
  void setTotal( T total ) { m_Total = total; }

  /**
   * @brief get the total amount of the goods
   * @return
   */
  T getTotal() const { return m_Total; }

  /**
   * @brief set the currency of the goods
   * @param currency
   */
  void setCurrency( std::string_view currency ) { m_Currency = currency.data(); }

  /**
   * @brief get the currency of the goods
   * @return the currency of the goods
   */
  std::string getCurrency() const { return m_Currency; }

 private:
  int m_Id {};
  std::string m_Name {};
  int m_Amount {};
  T m_Price {};
  T m_Total {};
  std::string m_Currency {};
};

};  // namespace GoodsTrack
