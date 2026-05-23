#ifndef PARSING_HPP
#define PARSING_HPP

#include <cstddef>
#include <string>

#include "../common/list.hpp"

namespace muraviev
{
  using TokenList = List< std::string >;

  bool splitStrictSpaces(const std::string& line, TokenList& tokens);
  bool parseNonNegativeLongLong(const std::string& text, long long& value);
  bool parsePositiveLongLong(const std::string& text, long long& value);
  bool parseNonNegativeSize(const std::string& text, size_t& value);
  bool parsePositiveSize(const std::string& text, size_t& value);
  size_t countTokens(const TokenList& tokens);
  const std::string& tokenAt(const TokenList& tokens, size_t index);
}

#endif
