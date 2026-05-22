#include "commands.hpp"

#include <fstream>
#include <iostream>

#include "parsing.hpp"

namespace
{
  using Tokens = muraviev::TokenList;
  using CommandHandler = bool (*)(muraviev::DatasetTable&, const Tokens&,
      std::ostream&);
  using CommandTable = muraviev::BSTree< std::string, CommandHandler,
      muraviev::Less< std::string > >;

  const std::string& tokenAt(const Tokens& tokens, size_t index)
  {
    size_t current = 0;
    for (Tokens::c_iter it = tokens.begin(); it != tokens.end(); ++it) {
      if (current == index) {
        return *it;
      }
      ++current;
    }
    throw std::out_of_range("token not found");
  }

  bool printCommand(muraviev::DatasetTable& datasets, const Tokens& tokens,
      std::ostream& output)
  {
    if (muraviev::countTokens(tokens) != 2 || !datasets.contains(tokenAt(tokens, 1))) {
      return false;
    }

    const std::string& name = tokenAt(tokens, 1);
    const muraviev::Dataset& dataset = datasets.get(name);
    if (dataset.empty()) {
      output << "<EMPTY>\n";
      return true;
    }

    output << name;
    for (muraviev::Dataset::const_iterator it = dataset.cbegin();
        it != dataset.cend(); ++it) {
      output << ' ' << it->key << ' ' << it->value;
    }
    output << '\n';
    return true;
  }

  bool complementCommand(muraviev::DatasetTable& datasets, const Tokens& tokens,
      std::ostream&)
  {
    if (muraviev::countTokens(tokens) != 4 || datasets.contains(tokenAt(tokens, 1)) ||
        !datasets.contains(tokenAt(tokens, 2)) || !datasets.contains(tokenAt(tokens, 3))) {
      return false;
    }

    const muraviev::Dataset& left = datasets.get(tokenAt(tokens, 2));
    const muraviev::Dataset& right = datasets.get(tokenAt(tokens, 3));
    muraviev::Dataset result;
    for (muraviev::Dataset::const_iterator it = left.cbegin(); it != left.cend(); ++it) {
      if (!right.contains(it->key)) {
        result.push(it->key, it->value);
      }
    }
    datasets.push(tokenAt(tokens, 1), result);
    return true;
  }

  bool intersectCommand(muraviev::DatasetTable& datasets, const Tokens& tokens,
      std::ostream&)
  {
    if (muraviev::countTokens(tokens) != 4 || datasets.contains(tokenAt(tokens, 1)) ||
        !datasets.contains(tokenAt(tokens, 2)) || !datasets.contains(tokenAt(tokens, 3))) {
      return false;
    }

    const muraviev::Dataset& left = datasets.get(tokenAt(tokens, 2));
    const muraviev::Dataset& right = datasets.get(tokenAt(tokens, 3));
    muraviev::Dataset result;
    for (muraviev::Dataset::const_iterator it = left.cbegin(); it != left.cend(); ++it) {
      if (right.contains(it->key)) {
        result.push(it->key, it->value);
      }
    }
    datasets.push(tokenAt(tokens, 1), result);
    return true;
  }

  bool unionCommand(muraviev::DatasetTable& datasets, const Tokens& tokens,
      std::ostream&)
  {
    if (muraviev::countTokens(tokens) != 4 || datasets.contains(tokenAt(tokens, 1)) ||
        !datasets.contains(tokenAt(tokens, 2)) || !datasets.contains(tokenAt(tokens, 3))) {
      return false;
    }

    const muraviev::Dataset& left = datasets.get(tokenAt(tokens, 2));
    const muraviev::Dataset& right = datasets.get(tokenAt(tokens, 3));
    muraviev::Dataset result;
    for (muraviev::Dataset::const_iterator it = left.cbegin(); it != left.cend(); ++it) {
      result.push(it->key, it->value);
    }
    for (muraviev::Dataset::const_iterator it = right.cbegin(); it != right.cend(); ++it) {
      if (!result.contains(it->key)) {
        result.push(it->key, it->value);
      }
    }
    datasets.push(tokenAt(tokens, 1), result);
    return true;
  }

  CommandTable createCommandTable()
  {
    CommandTable commands;
    commands.push("print", printCommand);
    commands.push("complement", complementCommand);
    commands.push("intersect", intersectCommand);
    commands.push("union", unionCommand);
    return commands;
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

    Tokens tokens;
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

void muraviev::executeCommands(std::istream& input, std::ostream& output,
    DatasetTable& datasets)
{
  CommandTable commands = createCommandTable();
  std::string line;
  while (std::getline(input, line)) {
    if (line.empty()) {
      continue;
    }

    Tokens tokens;
    if (!splitStrictSpaces(line, tokens) || tokens.empty() ||
        !commands.contains(tokenAt(tokens, 0))) {
      output << "<INVALID COMMAND>\n";
      continue;
    }

    const CommandHandler handler = commands.get(tokenAt(tokens, 0));
    if (!handler(datasets, tokens, output)) {
      output << "<INVALID COMMAND>\n";
    }
  }
}
