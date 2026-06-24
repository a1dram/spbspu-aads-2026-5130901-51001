#ifndef STORAGE_HPP
#define STORAGE_HPP

#include <iosfwd>
#include <string>

#include "models.hpp"

namespace muraviev
{
  enum TransferResult
  {
    transferOk,
    transferInvalid,
    transferOverflow
  };

  class CommandContext
  {
  public:
    CommandContext();

    WalletTree& wallets();
    const WalletTree& wallets() const;
    TransferTree& transfers();
    const TransferTree& transfers() const;
    TransferLog& transferLog();
    const TransferLog& transferLog() const;

    bool addWallet(const std::string& address, const std::string& label,
        long long balance);
    bool dropWallet(const std::string& address);
    TransferResult makeTransfer(const std::string& id, const std::string& from,
        const std::string& to, long long amount);
    void clear();
    void setNextOrder(size_t order);
    size_t nextOrder() const;

  private:
    WalletTree wallets_;
    TransferTree transfers_;
    TransferLog transferLog_;
    size_t nextOrder_;
  };

  bool saveContext(const CommandContext& context, const std::string& filename);
  bool loadContext(CommandContext& context, const std::string& filename);
}

#endif
