#include "parsing.hpp"

#include <limits>

bool muraviev::isValidName(const std::string& value)
{
  if (value.empty()) {
    return false;
  }
  const char first = value[0];
  if (!((first >= 'A' && first <= 'Z') || (first >= 'a' && first <= 'z'))) {
    return false;
  }
  for (size_t i = 1; i < value.size(); ++i) {
    const char c = value[i];
    const bool isLetter = (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
    const bool isDigit = c >= '0' && c <= '9';
    if (!isLetter && !isDigit) {
      return false;
    }
  }
  return true;
}

bool muraviev::splitStrictSpaces(const std::string& line,
    std::vector< std::string >& tokens)
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
      tokens.push_back(current);
      current.clear();
    } else {
      current += c;
    }
  }

  if (!current.empty()) {
    tokens.push_back(current);
  }
  return true;
}

bool muraviev::parseUnsignedLongLong(const std::string& text,
    unsigned long long& value)
{
  if (text.empty() || text[0] == '-') {
    return false;
  }

  unsigned long long result = 0;
  const unsigned long long max = std::numeric_limits< unsigned long long >::max();
  for (size_t i = 0; i < text.size(); ++i) {
    const char c = text[i];
    if (c < '0' || c > '9') {
      return false;
    }
    const unsigned int digit = static_cast< unsigned int >(c - '0');
    if (result > (max - digit) / 10) {
      return false;
    }
    result = result * 10 + digit;
  }

  value = result;
  return true;
}

bool muraviev::parseSize(const std::string& text, size_t& value)
{
  unsigned long long parsed = 0;
  if (!parseUnsignedLongLong(text, parsed)) {
    return false;
  }
  if (parsed > std::numeric_limits< size_t >::max()) {
    return false;
  }
  value = static_cast< size_t >(parsed);
  return true;
}
