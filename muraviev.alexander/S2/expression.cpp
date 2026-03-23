#include "expression.hpp"

namespace muraviev {
  TokenArray::TokenArray():
    data(nullptr),
    size(0)
  {}

  TokenArray::TokenArray(const TokenArray& other):
    data(nullptr),
    size(0)
  {
    if (other.size == 0) {
      return;
    }

    data = new std::string[other.size];
    size = other.size;
    for (size_t i = 0; i < size; ++i) {
      data[i] = other.data[i];
    }
  }

  TokenArray& TokenArray::operator=(const TokenArray& other)
  {
    if (this == &other) {
      return *this;
    }

    delete[] data;
    data = nullptr;
    size = 0;

    if (other.size == 0) {
      return *this;
    }

    data = new std::string[other.size];
    size = other.size;
    for (size_t i = 0; i < size; ++i) {
      data[i] = other.data[i];
    }

    return *this;
  }

  TokenArray::~TokenArray()
  {
    delete[] data;
  }

  void TokenArray::pushBack(const std::string& value)
  {
    std::string* newData = new std::string[size + 1];
    for (size_t i = 0; i < size; ++i) {
      newData[i] = data[i];
    }
    newData[size] = value;

    delete[] data;
    data = newData;
    ++size;
  }

  TokenArray splitTokens(const std::string& line)
  {
    TokenArray tokens;
    std::string token;

    for (size_t i = 0; i < line.size(); ++i)
    {
      char c = line[i];

      if (c != ' ') {
        token += c;
      } else if (!token.empty()) {
        tokens.pushBack(token);
        token.clear();
      }
    }

    if (!token.empty()) {
      tokens.pushBack(token);
    }

    return tokens;
  }
}
