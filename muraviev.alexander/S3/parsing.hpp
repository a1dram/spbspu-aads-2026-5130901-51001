#ifndef PARSING_HPP
#define PARSING_HPP

#include <cstddef>
#include <string>
#include <vector>

namespace muraviev
{
  bool isValidName(const std::string& value);
  bool splitStrictSpaces(const std::string& line, std::vector< std::string >& tokens);
  bool parseUnsignedLongLong(const std::string& text, unsigned long long& value);
  bool parseSize(const std::string& text, size_t& value);
}

#endif
