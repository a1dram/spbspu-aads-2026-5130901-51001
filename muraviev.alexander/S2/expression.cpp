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

    size_t i = 0;
    if (token[0] == '-') {
      if (token.size() == 1) {
        return false;
      }
      i = 1;
    }

    for (; i < token.size(); ++i) {
      char c = token[i];
      if (c < '0' || c > '9') {
        return false;
      }
    }

    return true;
  }

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
        bool found = false;
        while (!opStack.empty()) {
          std::string op = opStack.drop();
          if (op == "(") {
            found = true;
            break;
          }
          output.push(op);
        }
        if (!found) {
          throw std::logic_error("mismatched parentheses");
        }
        continue;
      }

      if (isOperator(t)) {
        while (!opStack.empty() && isOperator(opStack.top())) {
          std::string topOp = opStack.top();
          int pTop = getPriority(topOp);
          int pCur = getPriority(t);

          bool popIt = false;
          if (t == "**") {
            popIt = pTop > pCur;
          } else {
            popIt = pTop >= pCur;
          }

          if (!popIt) {
            break;
          }

          output.push(opStack.drop());
        }
        opStack.push(t);
        continue;
      }

      throw std::logic_error("unknown token");
    }

    while (!opStack.empty()) {
      std::string op = opStack.drop();
      if (op == "(" || op == ")") {
        throw std::logic_error("mismatched parentheses");
      }
      output.push(op);
    }

    return output;
  }

  long long convertToLongLong(const std::string& token)
  {
    bool isNegative = false;
    size_t i = 0;
    if (token[0] == '-') {
      isNegative = true;
      i = 1;
    }

    long long value = 0;
    for (; i < token.size(); ++i) {
      value = value * 10 + (token[i] - '0');
    }

    return isNegative ? -value : value;
  }

  long long toExponentiate(long long base, long long exp)
  {
    long long result = 1;
    for (long long i = 0; i < exp; ++i) {
      result *= base;
    }
    return result;
  }

  long long evaluatePostfix(Queue< std::string > postfix)
  {
    Stack< long long > st;

    while (!postfix.empty()) {
      std::string t = postfix.drop();

      if (isNumber(t)) {
        st.push(convertToLongLong(t));
        continue;
      }

      if (!isOperator(t)) {
        throw std::logic_error("unknown token");
      }

      long long right = st.drop();
      long long left = st.drop();

      if (t == "+") {
        st.push(left + right);
      } else if (t == "-") {
        st.push(left - right);
      } else if (t == "*") {
        st.push(left * right);
      } else if (t == "/") {
        st.push(left / right);
      } else if (t == "%") {
        st.push(left % right);
      } else if (t == "**") {
        st.push(toExponentiate(left, right));
      } else {
        throw std::logic_error("unknown operator");
      }
    }

    if (st.empty()) {
      throw std::logic_error("empty expression");
    }

    long long result = st.drop();
    return result;
  }
}
