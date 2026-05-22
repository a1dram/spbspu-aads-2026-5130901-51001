#ifndef PARSING_HPP
#define PARSING_HPP

#include <string>

#include "../common/list.hpp"

namespace muraviev
{
  using TokenList = List< std::string >;

  bool splitStrictSpaces(const std::string& line, TokenList& tokens);
  size_t countTokens(const TokenList& tokens);
}

#endif
