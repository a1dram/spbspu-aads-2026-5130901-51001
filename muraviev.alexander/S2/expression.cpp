#include "expression.hpp"

#include <stdexcept>

#include "stack.hpp"

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

  bool isNumber(const std::string& token)
  {
    if (token.empty()) {
      return false;
    }

    for (size_t i = 0; i < token.size(); ++i) {
      char c = token[i];
      if (c < '0' || c > '9') {
        return false;
      }
    }

    return true;
  }

  #include <stdexcept>

  bool isOperator(const std::string& token)
  {
    return (token == "+" || token == "-" || token == "*" ||
       token == "/" || token == "%" || token == "**");
  }

  int getPriority(const std::string& op)
  {
    if (op == "**") {
      return 3;
    }
    if (op == "*" || op == "/" || op == "%") {
      return 2;
    }
    if (op == "+" || op == "-") {
      return 1;
    }

    throw std::logic_error("unknown operator");
  }

  Queue< std::string > infixToPostfix(const TokenArray& tokens)
  {
    Stack< std::string > opStack;
    Queue< std::string > output;

    for (size_t i = 0; i < tokens.size; ++i) {
      const std::string& t = tokens.data[i];

      if (isNumber(t)) {
        output.push(t);
        continue;
      }

      if (t == "(") {
        opStack.push(t);
        continue;
      }

      if (t == ")") {
        while (!opStack.empty() && opStack.top() != "(") {
          output.push(opStack.drop());
        }
        if (!opStack.empty()) {
          opStack.drop();
        }
        continue;
      }

      if (isOperator(t)) {
        while (!opStack.empty() && isOperator(opStack.top()) &&
               getPriority(opStack.top()) >= getPriority(t)) {
          output.push(opStack.drop());
        }
        opStack.push(t);
        continue;
      }

      throw std::logic_error("unknown token");
    }

    while (!opStack.empty()) {
      output.push(opStack.drop());
    }

    return output;
  }

}
