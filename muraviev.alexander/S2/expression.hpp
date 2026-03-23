#ifndef EXPRESSION_HPP
#define EXPRESSION_HPP

#include <string>
#include <limits>

#include "queue.hpp"

namespace muraviev
{
  using ll_t = long long;

  inline const ll_t LL_MAX = std::numeric_limits< ll_t >::max();
  inline const ll_t LL_MIN = std::numeric_limits< ll_t >::min();

  struct TokenArray
  {
    std::string* data;
    size_t size;

    TokenArray();
    TokenArray(const TokenArray& other);
    TokenArray& operator=(const TokenArray& other);
    ~TokenArray();

    void pushBack(const std::string& value);
  };

  TokenArray splitTokens(const std::string& line);

  ll_t add(const ll_t& a, const ll_t& b);
  ll_t sub(const ll_t& a, const ll_t& b);
  ll_t mul(const ll_t& a, const ll_t& b);
  ll_t div(const ll_t& a, const ll_t& b);
  ll_t mod(const ll_t& a, const ll_t& b);

  bool isNumber(const std::string& token);
  bool isOperator(const std::string& token);

  int getPriority(const std::string& op);

  Queue< std::string > infixToPostfix(const TokenArray& tokens);

  ll_t convertToLongLong(const std::string& token);
  ll_t toExponentiate(ll_t base, ll_t exp);
  ll_t evaluatePostfix(Queue< std::string > postfix);
}

#endif
