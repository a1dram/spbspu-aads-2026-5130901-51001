#include "commands.hpp"

#include <algorithm>
#include <fstream>
#include <sstream>
#include <vector>

#include "parsing.hpp"

namespace
{
  using Tokens = muraviev::TokenList;
  using CommandHandler = bool (*)(muraviev::CommandContext&, const Tokens&,
      std::ostream&);
  using CommandTable = muraviev::RBTree< std::string, CommandHandler,
      muraviev::Less< std::string > >;
  using StringSet = muraviev::RBTree< std::string, bool,
      muraviev::Less< std::string > >;

  bool makeWalletCommand(muraviev::CommandContext& context, const Tokens& tokens,
      std::ostream&)
  {
    long long balance = 0;
    return muraviev::countTokens(tokens) == 4 &&
        muraviev::parseNonNegativeLongLong(muraviev::tokenAt(tokens, 3), balance) &&
        context.addWallet(muraviev::tokenAt(tokens, 1), muraviev::tokenAt(tokens, 2),
            balance);
  }

  CommandTable createCommandTable()
  {
    CommandTable commands;
    commands.push("make-wallet", makeWalletCommand);
    return commands;
  }
}

void muraviev::executeCommands(std::istream& input, std::ostream& output,
    CommandContext& context)
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
    if (!handler(context, tokens, output)) {
      output << "<INVALID COMMAND>\n";
    }
  }
}
