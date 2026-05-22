#include "parsing.hpp"

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

size_t muraviev::countTokens(const TokenList& tokens)
{
  size_t count = 0;
  for (TokenList::c_iter it = tokens.begin(); it != tokens.end(); ++it) {
    ++count;
  }
  return count;
}
