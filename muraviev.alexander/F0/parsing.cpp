#include "parsing.hpp"

bool muraviev::splitStrictSpaces(const std::string& line, TokenList& tokens)
{
  tokens.clear();
  std::string current;
  for (size_t i = 0; i < line.size(); ++i) {
    if (line[i] == ' ') {
      if (!current.empty()) {
        if (tokens.empty()) {
          tokens.pushFront(current);
        } else {
          tokens.insert(tokens.last(), current);
        }
        current.clear();
      }
    } else {
      current += line[i];
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
