#include "storage.hpp"

#include <fstream>
#include <limits>

#include "parsing.hpp"

muraviev::Wallet::Wallet():
  address(),
  label(),
  balance(0),
  inCount(0),
  outCount(0),
  inSum(0),
  outSum(0)
{}

muraviev::Wallet::Wallet(const std::string& walletAddress,
    const std::string& walletLabel, long long walletBalance):
  address(walletAddress),
  label(walletLabel),
  balance(walletBalance),
  inCount(0),
  outCount(0),
  inSum(0),
  outSum(0)
{}

muraviev::Transfer::Transfer():
  id(),
  fromAddress(),
  toAddress(),
  amount(0),
  order(0)
{}

muraviev::Transfer::Transfer(const std::string& transferId,
    const std::string& from, const std::string& to, long long transferAmount,
    size_t transferOrder):
  id(transferId),
  fromAddress(from),
  toAddress(to),
  amount(transferAmount),
  order(transferOrder)
{}

muraviev::CommandContext::CommandContext():
  wallets_(),
  transfers_(),
  transferLog_(),
  nextOrder_(1)
{}

muraviev::WalletTree& muraviev::CommandContext::wallets()
{
  return wallets_;
}

const muraviev::WalletTree& muraviev::CommandContext::wallets() const
{
  return wallets_;
}

muraviev::TransferTree& muraviev::CommandContext::transfers()
{
  return transfers_;
}

const muraviev::TransferTree& muraviev::CommandContext::transfers() const
{
  return transfers_;
}

muraviev::TransferLog& muraviev::CommandContext::transferLog()
{
  return transferLog_;
}

const muraviev::TransferLog& muraviev::CommandContext::transferLog() const
{
  return transferLog_;
}

bool muraviev::CommandContext::addWallet(const std::string& address,
    const std::string& label, long long balance)
{
  if (wallets_.contains(address) || balance < 0) {
    return false;
  }
  wallets_.push(address, Wallet(address, label, balance));
  return true;
}

bool muraviev::CommandContext::dropWallet(const std::string& address)
{
  if (!wallets_.contains(address)) {
    return false;
  }
  const Wallet& wallet = wallets_.get(address);
  if (wallet.inCount != 0 || wallet.outCount != 0) {
    return false;
  }
  wallets_.drop(address);
  return true;
}

muraviev::TransferResult muraviev::CommandContext::makeTransfer(
    const std::string& id,
    const std::string& from, const std::string& to, long long amount)
{
  if (transfers_.contains(id) || !wallets_.contains(from) ||
      !wallets_.contains(to) || amount <= 0 || from == to) {
    return transferInvalid;
  }
  Wallet& source = wallets_.get(from);
  Wallet& target = wallets_.get(to);
  if (source.balance < amount) {
    return transferInvalid;
  }

  const long long longMax = std::numeric_limits< long long >::max();
  const size_t sizeMax = std::numeric_limits< size_t >::max();
  if (target.balance > longMax - amount ||
      source.outSum > longMax - amount ||
      target.inSum > longMax - amount ||
      source.outCount == sizeMax ||
      target.inCount == sizeMax ||
      nextOrder_ == sizeMax) {
    return transferOverflow;
  }

  source.balance -= amount;
  source.outCount += 1;
  source.outSum += amount;
  target.balance += amount;
  target.inCount += 1;
  target.inSum += amount;

  const Transfer transfer(id, from, to, amount, nextOrder_);
  transfers_.push(id, transfer);
  if (transferLog_.empty()) {
    transferLog_.pushFront(transfer);
  } else {
    transferLog_.insert(transferLog_.last(), transfer);
  }
  ++nextOrder_;
  return transferOk;
}

void muraviev::CommandContext::clear()
{
  wallets_.clear();
  transfers_.clear();
  transferLog_.clear();
  nextOrder_ = 1;
}

void muraviev::CommandContext::setNextOrder(size_t order)
{
  nextOrder_ = order;
}

size_t muraviev::CommandContext::nextOrder() const
{
  return nextOrder_;
}

bool muraviev::saveContext(const CommandContext& context,
    const std::string& filename)
{
  std::ofstream output(filename.c_str());
  if (!output) {
    return false;
  }
  output << "WALLETS " << context.wallets().size() << '\n';
  for (WalletTree::const_iterator it = context.wallets().cbegin();
      it != context.wallets().cend(); ++it) {
    const Wallet& wallet = it->value;
    output << "W " << wallet.address << ' ' << wallet.label << ' '
        << wallet.balance << ' ' << wallet.inCount << ' ' << wallet.outCount
        << ' ' << wallet.inSum << ' ' << wallet.outSum << '\n';
  }
  output << "TRANSFERS " << context.transfers().size() << '\n';
  for (TransferLog::c_iter it = context.transferLog().begin();
      it != context.transferLog().end(); ++it) {
    output << "T " << it->id << ' ' << it->fromAddress << ' '
        << it->toAddress << ' ' << it->amount << ' ' << it->order << '\n';
  }
  return static_cast< bool >(output);
}

bool muraviev::loadContext(CommandContext& context, const std::string& filename)
{
  std::ifstream input(filename.c_str());
  if (!input) {
    return false;
  }

  CommandContext loaded;
  std::string line;
  TokenList tokens;
  size_t walletCount = 0;
  size_t transferCount = 0;
  if (!std::getline(input, line) || !splitStrictSpaces(line, tokens) ||
      countTokens(tokens) != 2 || tokenAt(tokens, 0) != "WALLETS" ||
      !parseNonNegativeSize(tokenAt(tokens, 1), walletCount)) {
    return false;
  }

  for (size_t i = 0; i < walletCount; ++i) {
    long long balance = 0;
    long long inSum = 0;
    long long outSum = 0;
    size_t inCount = 0;
    size_t outCount = 0;
    if (!std::getline(input, line) || !splitStrictSpaces(line, tokens) ||
        countTokens(tokens) != 8 || tokenAt(tokens, 0) != "W" ||
        loaded.wallets().contains(tokenAt(tokens, 1)) ||
        !parseNonNegativeLongLong(tokenAt(tokens, 3), balance) ||
        !parseNonNegativeSize(tokenAt(tokens, 4), inCount) ||
        !parseNonNegativeSize(tokenAt(tokens, 5), outCount) ||
        !parseNonNegativeLongLong(tokenAt(tokens, 6), inSum) ||
        !parseNonNegativeLongLong(tokenAt(tokens, 7), outSum)) {
      return false;
    }
    Wallet wallet(tokenAt(tokens, 1), tokenAt(tokens, 2), balance);
    wallet.inCount = inCount;
    wallet.outCount = outCount;
    wallet.inSum = inSum;
    wallet.outSum = outSum;
    loaded.wallets().push(wallet.address, wallet);
  }

  if (!std::getline(input, line) || !splitStrictSpaces(line, tokens) ||
      countTokens(tokens) != 2 || tokenAt(tokens, 0) != "TRANSFERS" ||
      !parseNonNegativeSize(tokenAt(tokens, 1), transferCount)) {
    return false;
  }

  size_t maxOrder = 0;
  for (size_t i = 0; i < transferCount; ++i) {
    long long amount = 0;
    size_t order = 0;
    if (!std::getline(input, line) || !splitStrictSpaces(line, tokens) ||
        countTokens(tokens) != 6 || tokenAt(tokens, 0) != "T" ||
        loaded.transfers().contains(tokenAt(tokens, 1)) ||
        !loaded.wallets().contains(tokenAt(tokens, 2)) ||
        !loaded.wallets().contains(tokenAt(tokens, 3)) ||
        tokenAt(tokens, 2) == tokenAt(tokens, 3) ||
        !parsePositiveLongLong(tokenAt(tokens, 4), amount) ||
        !parsePositiveSize(tokenAt(tokens, 5), order)) {
      return false;
    }
    const Transfer transfer(tokenAt(tokens, 1), tokenAt(tokens, 2),
        tokenAt(tokens, 3), amount, order);
    loaded.transfers().push(transfer.id, transfer);
    if (loaded.transferLog().empty()) {
      loaded.transferLog().pushFront(transfer);
    } else {
      loaded.transferLog().insert(loaded.transferLog().last(), transfer);
    }
    if (order > maxOrder) {
      maxOrder = order;
    }
  }
  if (std::getline(input, line)) {
    return false;
  }
  if (maxOrder == std::numeric_limits< size_t >::max()) {
    return false;
  }
  loaded.setNextOrder(maxOrder + 1);
  context = loaded;
  return true;
}
