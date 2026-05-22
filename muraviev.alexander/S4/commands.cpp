#include "commands.hpp"

#include <fstream>

#include "parsing.hpp"

namespace
{
  const std::string& tokenAt(const muraviev::TokenList& tokens, size_t index)
  {
    size_t current = 0;
    for (muraviev::TokenList::c_iter it = tokens.begin(); it != tokens.end(); ++it) {
      if (current == index) {
        return *it;
      }
      ++current;
    }
    throw std::out_of_range("token not found");
  }
}

bool muraviev::loadDatasets(const std::string& filename, DatasetTable& datasets)
{
  std::ifstream input(filename.c_str());
  if (!input) {
    return false;
  }

  std::string line;
  while (std::getline(input, line)) {
    if (line.empty()) {
      continue;
    }

    TokenList tokens;
    if (!splitStrictSpaces(line, tokens) || countTokens(tokens) != 1) {
      return false;
    }
    const std::string name = tokenAt(tokens, 0);
    if (datasets.contains(name)) {
      return false;
    }
    datasets.push(name, Dataset());
  }
  return true;
}
