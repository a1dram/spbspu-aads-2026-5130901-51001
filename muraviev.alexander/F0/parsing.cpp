#include "parsing.hpp"

#include <limits>
#include <stdexcept>

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
    const char c = line[i];
    if (c == ' ') {
      if (current.empty()) {
        return false;
      }
      if (tokens.empty()) {
        tokens.pushFront(current);
      } else {
        tokens.insert(tokens.last(), current);
      }
      current.clear();
    } else {
      current += c;
    }
  }
  if (!current.empty()) {
    if (tokens.empty()) {
      tokens.pushFront(current);
    } else {
      tokens.insert(tokens.last(), current);
    }
  }
  return true;
}

bool muraviev::parseNonNegativeLongLong(const std::string& text, long long& value)
{
  if (text.empty()) {
    return false;
  }
  long long result = 0;
  for (size_t i = 0; i < text.size(); ++i) {
    const char c = text[i];
    if (c < '0' || c > '9') {
      return false;
    }
    const int digit = c - '0';
    if (result > (std::numeric_limits< long long >::max() - digit) / 10) {
      return false;
    }
    result = result * 10 + digit;
  }
  value = result;
  return true;
}

bool muraviev::parsePositiveLongLong(const std::string& text, long long& value)
{
  return parseNonNegativeLongLong(text, value) && value > 0;
}

bool muraviev::parseNonNegativeSize(const std::string& text, size_t& value)
{
  long long parsed = 0;
  if (!parseNonNegativeLongLong(text, parsed)) {
    return false;
  }
  if (static_cast< unsigned long long >(parsed) >
      std::numeric_limits< size_t >::max()) {
    return false;
  }
  value = static_cast< size_t >(parsed);
  return true;
}

bool muraviev::parsePositiveSize(const std::string& text, size_t& value)
{
  long long parsed = 0;
  if (!parsePositiveLongLong(text, parsed)) {
    return false;
  }
  value = static_cast< size_t >(parsed);
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

const std::string& muraviev::tokenAt(const TokenList& tokens, size_t index)
{
  size_t current = 0;
  for (TokenList::c_iter it = tokens.begin(); it != tokens.end(); ++it) {
    if (current == index) {
      return *it;
    }
    ++current;
  }
  throw std::out_of_range("token not found");
}
