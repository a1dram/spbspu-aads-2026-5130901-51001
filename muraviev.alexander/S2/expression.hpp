#ifndef EXPRESSION_HPP
#define EXPRESSION_HPP

#include <string>

#include "queue.hpp"

namespace muraviev
{
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

  bool isNumber(const std::string& token);
  bool isOperator(const std::string& token);

  int getPriority(const std::string& op);

  Queue< std::string > infixToPostfix(const TokenArray& tokens);
}

#endif
