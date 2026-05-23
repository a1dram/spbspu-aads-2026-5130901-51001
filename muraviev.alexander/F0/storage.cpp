#include "storage.hpp"

#include <fstream>

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

bool muraviev::CommandContext::makeTransfer(const std::string& id,
    const std::string& from, const std::string& to, long long amount)
{
  if (transfers_.contains(id) || !wallets_.contains(from) ||
      !wallets_.contains(to) || amount <= 0 || from == to) {
    return false;
  }
  Wallet& source = wallets_.get(from);
  Wallet& target = wallets_.get(to);
  if (source.balance < amount) {
    return false;
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
  return true;
}
