#ifndef EXPRESSION_HPP
#define EXPRESSION_HPP

#include <string>

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
}

#endif
