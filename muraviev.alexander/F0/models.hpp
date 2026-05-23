#ifndef MODELS_HPP
#define MODELS_HPP

#include <cstddef>
#include <string>

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
  };

  struct Transfer
  {
    std::string id;
    std::string fromAddress;
    std::string toAddress;
    long long amount;
    size_t order;
  };
}

#endif
