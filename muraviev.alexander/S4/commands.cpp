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

  const std::string& tokenAt(const Tokens& tokens, size_t index);
  bool printCommand(muraviev::DatasetTable& datasets, const Tokens& tokens,
      std::ostream& output)
  {
    if (muraviev::countTokens(tokens) != 2 || !datasets.contains(tokenAt(tokens, 1))) {
      return false;
    }

    const std::string& name = tokenAt(tokens, 1);
    const muraviev::Dataset& dataset = datasets.get(name);
    output << name;
    for (muraviev::Dataset::const_iterator it = dataset.cbegin();
        it != dataset.cend(); ++it) {
      output << ' ' << it->key << ' ' << it->value;
    }
    output << '\n';
    return true;
  }
}

void muraviev::executeCommands(std::istream& input, std::ostream& output,
    DatasetTable& datasets)
{
  CommandTable commands;
  commands.push("print", printCommand);

  std::string line;
  while (std::getline(input, line)) {
    Tokens tokens;
    if (!splitStrictSpaces(line, tokens) || tokens.empty() ||
        !commands.contains(tokenAt(tokens, 0))) {
      output << "<INVALID COMMAND>\n";
      continue;
    }
    if (!commands.get(tokenAt(tokens, 0))(datasets, tokens, output)) {
      output << "<INVALID COMMAND>\n";
    }
  }
}
