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
