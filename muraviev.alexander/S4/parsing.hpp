#ifndef PARSING_HPP
#define PARSING_HPP

#include <cstddef>
#include <string>

#include "../common/list.hpp"

namespace muraviev
{
  using TokenList = List< std::string >;

  bool splitStrictSpaces(const std::string& line, TokenList& tokens);
  bool parseInt(const std::string& text, int& value);
  size_t countTokens(const TokenList& tokens);
}

#endif
