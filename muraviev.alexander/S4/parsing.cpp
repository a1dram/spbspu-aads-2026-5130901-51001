#include "parsing.hpp"

#include <limits>

bool muraviev::splitStrictSpaces(const std::string& line, TokenList& tokens)
{
  tokens.clear();
  if (line.empty()) {
    return true;
  }
  if (line[0] == ' ' || line[line.size() - 1] == ' ') {
    return false;
  }

  std::string current;
  for (size_t i = 0; i < line.size(); ++i) {
    if (line[i] == ' ') {
      if (current.empty()) {
        return false;
      }
      tokens.empty() ? tokens.pushFront(current) : tokens.insert(tokens.last(), current);
      current.clear();
    } else {
      current += line[i];
    }
  }

  if (!current.empty()) {
    tokens.empty() ? tokens.pushFront(current) : tokens.insert(tokens.last(), current);
  }
  return true;
}

bool muraviev::parseInt(const std::string& text, int& value)
{
  if (text.empty()) {
    return false;
  }

  size_t index = 0;
  const bool negative = text[0] == '-';
  if (negative) {
    if (text.size() == 1) {
      return false;
    }
    index = 1;
  }

  long long result = 0;
  const long long limit = negative ?
      -static_cast< long long >(std::numeric_limits< int >::min()) :
      std::numeric_limits< int >::max();
  for (; index < text.size(); ++index) {
    if (text[index] < '0' || text[index] > '9') {
      return false;
    }
    const int digit = text[index] - '0';
    if (result > (limit - digit) / 10) {
      return false;
    }
    result = result * 10 + digit;
  }

  value = negative ? static_cast< int >(-result) : static_cast< int >(result);
  return true;
}

size_t muraviev::countTokens(const TokenList& tokens)
{
  size_t count = 0;
  for (TokenList::c_iter it = tokens.begin(); it != tokens.end(); ++it) {
    ++count;
  }
  return count;
}
