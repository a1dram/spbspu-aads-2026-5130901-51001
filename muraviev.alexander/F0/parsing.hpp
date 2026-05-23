#ifndef PARSING_HPP
#define PARSING_HPP

#include <string>
#include "../common/list.hpp"

namespace muraviev
{
  using TokenList = List< std::string >;
  bool splitStrictSpaces(const std::string& line, TokenList& tokens);
}

#endif
