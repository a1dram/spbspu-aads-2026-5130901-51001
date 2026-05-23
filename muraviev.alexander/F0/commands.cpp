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

  void printList(std::ostream& output, const std::vector< std::string >& values)
  {
    if (values.empty()) {
      output << "<NOTHING FOUND>\n";
      return;
    }
    output << '<';
    for (size_t i = 0; i < values.size(); ++i) {
      if (i != 0) {
        output << ", ";
      }
      output << values[i];
    }
    output << ">\n";
  }

  bool makeWalletCommand(muraviev::CommandContext& context, const Tokens& tokens,
      std::ostream&)
  {
    long long balance = 0;
    return muraviev::countTokens(tokens) == 4 &&
        muraviev::parseNonNegativeLongLong(muraviev::tokenAt(tokens, 3), balance) &&
        context.addWallet(muraviev::tokenAt(tokens, 1), muraviev::tokenAt(tokens, 2),
            balance);
  }

  bool showWalletCommand(muraviev::CommandContext& context, const Tokens& tokens,
      std::ostream& output)
  {
    if (muraviev::countTokens(tokens) != 2 ||
        !context.wallets().contains(muraviev::tokenAt(tokens, 1))) {
      return false;
    }
    const muraviev::Wallet& wallet = context.wallets().get(muraviev::tokenAt(tokens, 1));
    output << "<ADDRESS: " << wallet.address << ", LABEL: " << wallet.label
        << ", BALANCE: " << wallet.balance << ", IN-COUNT: " << wallet.inCount
        << ", OUT-COUNT: " << wallet.outCount << ", IN-SUM: " << wallet.inSum
        << ", OUT-SUM: " << wallet.outSum << ">\n";
    return true;
  }

  bool dropWalletCommand(muraviev::CommandContext& context, const Tokens& tokens,
      std::ostream&)
  {
    return muraviev::countTokens(tokens) == 2 &&
        context.dropWallet(muraviev::tokenAt(tokens, 1));
  }

  bool walletsCommand(muraviev::CommandContext& context, const Tokens& tokens,
      std::ostream& output)
  {
    if (muraviev::countTokens(tokens) != 1) {
      return false;
    }
    std::vector< std::string > addresses;
    for (muraviev::WalletTree::const_iterator it = context.wallets().cbegin();
        it != context.wallets().cend(); ++it) {
      addresses.push_back(it->key);
    }
    printList(output, addresses);
    return true;
  }

  bool makeTransferCommand(muraviev::CommandContext& context, const Tokens& tokens,
      std::ostream&)
  {
    long long amount = 0;
    return muraviev::countTokens(tokens) == 5 &&
        muraviev::parsePositiveLongLong(muraviev::tokenAt(tokens, 4), amount) &&
        context.makeTransfer(muraviev::tokenAt(tokens, 1), muraviev::tokenAt(tokens, 2),
            muraviev::tokenAt(tokens, 3), amount);
  }

  bool showTransferCommand(muraviev::CommandContext& context, const Tokens& tokens,
      std::ostream& output)
  {
    if (muraviev::countTokens(tokens) != 2 ||
        !context.transfers().contains(muraviev::tokenAt(tokens, 1))) {
      return false;
    }
    const muraviev::Transfer& transfer = context.transfers().get(muraviev::tokenAt(tokens, 1));
    output << "<ID: " << transfer.id << ", FROM: " << transfer.fromAddress
        << ", TO: " << transfer.toAddress << ", AMOUNT: " << transfer.amount
        << ">\n";
    return true;
  }

  bool transfersCommand(muraviev::CommandContext& context, const Tokens& tokens,
      std::ostream& output)
  {
    if (muraviev::countTokens(tokens) != 2 ||
        !context.wallets().contains(muraviev::tokenAt(tokens, 1))) {
      return false;
    }
    std::vector< std::string > ids;
    const std::string& address = muraviev::tokenAt(tokens, 1);
    for (muraviev::TransferTree::const_iterator it = context.transfers().cbegin();
        it != context.transfers().cend(); ++it) {
      if (it->value.fromAddress == address || it->value.toAddress == address) {
        ids.push_back(it->key);
      }
    }
    printList(output, ids);
    return true;
  }

  bool saveCommand(muraviev::CommandContext& context, const Tokens& tokens,
      std::ostream& output)
  {
    if (muraviev::countTokens(tokens) != 2 ||
        !muraviev::saveContext(context, muraviev::tokenAt(tokens, 1))) {
      return false;
    }
    output << "<OK>\n";
    return true;
  }

  bool loadCommand(muraviev::CommandContext& context, const Tokens& tokens,
      std::ostream&)
  {
    return muraviev::countTokens(tokens) == 2 &&
        muraviev::loadContext(context, muraviev::tokenAt(tokens, 1));
  }

  CommandTable createCommandTable()
  {
    CommandTable commands;
    commands.push("make-wallet", makeWalletCommand);
    commands.push("show-wallet", showWalletCommand);
    commands.push("drop-wallet", dropWalletCommand);
    commands.push("wallets", walletsCommand);
    commands.push("make-transfer", makeTransferCommand);
    commands.push("show-transfer", showTransferCommand);
    commands.push("transfers", transfersCommand);
    commands.push("save", saveCommand);
    commands.push("load", loadCommand);
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
