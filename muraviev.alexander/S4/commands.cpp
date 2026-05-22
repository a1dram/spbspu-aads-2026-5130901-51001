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
    if (!splitStrictSpaces(line, tokens) || tokens.empty()) {
      return false;
    }

    const std::string name = tokenAt(tokens, 0);
    const size_t tokensCount = countTokens(tokens);
    if (datasets.contains(name) || tokensCount % 2 == 0) {
      return false;
    }

    Dataset dataset;
    for (size_t i = 1; i < tokensCount; i += 2) {
      int key = 0;
      if (!parseInt(tokenAt(tokens, i), key)) {
        return false;
      }
      dataset.push(key, tokenAt(tokens, i + 1));
    }
    datasets.push(name, dataset);
  }
  return true;
}
