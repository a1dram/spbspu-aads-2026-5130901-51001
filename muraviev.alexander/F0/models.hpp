#ifndef MODELS_HPP
#define MODELS_HPP

#include <cstddef>
#include <string>

#include "../common/list.hpp"
#include "rb-tree.hpp"

namespace muraviev
{
  struct Wallet
  {
    std::string address;
    std::string label;
    long long balance;
    size_t inCount;
    size_t outCount;
    long long inSum;
    long long outSum;

    Wallet();
    Wallet(const std::string& walletAddress, const std::string& walletLabel,
        long long walletBalance);
  };

  struct Transfer
  {
    std::string id;
    std::string fromAddress;
    std::string toAddress;
    long long amount;
    size_t order;

    Transfer();
    Transfer(const std::string& transferId, const std::string& from,
        const std::string& to, long long transferAmount, size_t transferOrder);
  };

  using WalletTree = RBTree< std::string, Wallet, Less< std::string > >;
  using TransferTree = RBTree< std::string, Transfer, Less< std::string > >;
  using TransferLog = List< Transfer >;
}

#endif
