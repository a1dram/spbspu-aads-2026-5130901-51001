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

  struct HelpInfo
  {
    std::string usage;
    std::string description;
    std::string parameters;
    std::string example;
    std::string result;
    std::string fileFormat;
    std::string notes;

    HelpInfo():
      usage(),
      description(),
      parameters(),
      example(),
      result(),
      fileFormat(),
      notes()
    {}

    HelpInfo(const std::string& commandUsage, const std::string& commandDescription,
        const std::string& commandParameters, const std::string& commandExample,
        const std::string& commandResult, const std::string& commandNotes):
      usage(commandUsage),
      description(commandDescription),
      parameters(commandParameters),
      example(commandExample),
      result(commandResult),
      fileFormat(),
      notes(commandNotes)
    {}
  };

  using HelpTable = muraviev::RBTree< std::string, HelpInfo,
      muraviev::Less< std::string > >;

  struct QueueItem
  {
    std::string address;
    size_t depth;
  };

  struct WalletRank
  {
    std::string address;
    long long balance;
  };

  struct CycleInfo
  {
    std::vector< std::string > path;
    std::string key;
  };

  struct FlowState
  {
    std::string wallet;
    std::string branch;
    size_t depth;
    long long amount;
    int parent;
    std::string edgeFrom;
    std::string edgeTo;
    size_t edgeOrder;
    long long edgeAmount;
  };

  struct FlowEdge
  {
    std::string from;
    std::string to;
    long long amount;
    size_t order;
  };

  struct LaundryInfo
  {
    std::string source;
    std::string target;
    size_t branches;
    size_t depth;
    long long reached;
    long long score;
    std::vector< FlowEdge > edges;
  };

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

  std::vector< std::string > setToVector(const StringSet& set)
  {
    std::vector< std::string > result;
    for (StringSet::const_iterator it = set.cbegin(); it != set.cend(); ++it) {
      result.push_back(it->key);
    }
    return result;
  }

  std::vector< std::string > outgoing(const muraviev::CommandContext& context,
      const std::string& address)
  {
    StringSet set;
    for (muraviev::TransferLog::c_iter it = context.transferLog().begin();
        it != context.transferLog().end(); ++it) {
      if (it->fromAddress == address) {
        set.push(it->toAddress, true);
      }
    }
    return setToVector(set);
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
      std::ostream& output)
  {
    long long amount = 0;
    if (muraviev::countTokens(tokens) != 5 ||
        !muraviev::parsePositiveLongLong(muraviev::tokenAt(tokens, 4), amount)) {
      return false;
    }
    const muraviev::TransferResult result = context.makeTransfer(
        muraviev::tokenAt(tokens, 1), muraviev::tokenAt(tokens, 2),
        muraviev::tokenAt(tokens, 3), amount);
    if (result == muraviev::transferOverflow) {
      output << "<OVERFLOW ERROR>\n";
      return true;
    }
    return result == muraviev::transferOk;
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

  bool pathCommand(muraviev::CommandContext& context, const Tokens& tokens,
      std::ostream& output)
  {
    if (muraviev::countTokens(tokens) != 3 ||
        !context.wallets().contains(muraviev::tokenAt(tokens, 1)) ||
        !context.wallets().contains(muraviev::tokenAt(tokens, 2))) {
      return false;
    }
    const std::string from = muraviev::tokenAt(tokens, 1);
    const std::string to = muraviev::tokenAt(tokens, 2);
    StringSet visited;
    muraviev::RBTree< std::string, std::string, muraviev::Less< std::string > > prev;
    std::vector< QueueItem > queue;
    visited.push(from, true);
    queue.push_back({from, 0});
    for (size_t head = 0; head < queue.size(); ++head) {
      if (queue[head].address == to) {
        break;
      }
      const std::vector< std::string > next = outgoing(context, queue[head].address);
      for (size_t i = 0; i < next.size(); ++i) {
        if (!visited.contains(next[i])) {
          visited.push(next[i], true);
          prev.push(next[i], queue[head].address);
          queue.push_back({next[i], queue[head].depth + 1});
        }
      }
    }
    if (!visited.contains(to)) {
      output << "<NOTHING FOUND>\n";
      return true;
    }
    std::vector< std::string > reverse;
    for (std::string current = to; current != from; current = prev.get(current)) {
      reverse.push_back(current);
    }
    reverse.push_back(from);
    output << '<';
    for (size_t i = reverse.size(); i > 0; --i) {
      if (i != reverse.size()) {
        output << " -> ";
      }
      output << reverse[i - 1];
    }
    output << ">\n";
    return true;
  }

  bool relatedCommand(muraviev::CommandContext& context, const Tokens& tokens,
      std::ostream& output)
  {
    if (muraviev::countTokens(tokens) != 2 ||
        !context.wallets().contains(muraviev::tokenAt(tokens, 1))) {
      return false;
    }
    const std::string& address = muraviev::tokenAt(tokens, 1);
    StringSet in;
    StringSet out;
    for (muraviev::TransferLog::c_iter it = context.transferLog().begin();
        it != context.transferLog().end(); ++it) {
      if (it->toAddress == address) {
        in.push(it->fromAddress, true);
      }
      if (it->fromAddress == address) {
        out.push(it->toAddress, true);
      }
    }
    const std::vector< std::string > inList = setToVector(in);
    const std::vector< std::string > outList = setToVector(out);
    if (inList.empty() && outList.empty()) {
      output << "<NOTHING FOUND>\n";
      return true;
    }
    output << "<IN: ";
    for (size_t i = 0; i < inList.size(); ++i) {
      if (i != 0) {
        output << ", ";
      }
      output << inList[i];
    }
    output << "; OUT: ";
    for (size_t i = 0; i < outList.size(); ++i) {
      if (i != 0) {
        output << ", ";
      }
      output << outList[i];
    }
    output << ">\n";
    return true;
  }

  bool sinksCommand(muraviev::CommandContext& context, const Tokens& tokens,
      std::ostream& output)
  {
    size_t maxDepth = 0;
    if (muraviev::countTokens(tokens) != 3 ||
        !context.wallets().contains(muraviev::tokenAt(tokens, 1)) ||
        !muraviev::parsePositiveSize(muraviev::tokenAt(tokens, 2), maxDepth)) {
      return false;
    }
    const std::string start = muraviev::tokenAt(tokens, 1);
    StringSet visited;
    StringSet result;
    std::vector< QueueItem > queue;
    visited.push(start, true);
    queue.push_back({start, 0});
    for (size_t head = 0; head < queue.size(); ++head) {
      if (queue[head].depth == maxDepth) {
        continue;
      }
      const std::vector< std::string > next = outgoing(context, queue[head].address);
      for (size_t i = 0; i < next.size(); ++i) {
        if (!visited.contains(next[i])) {
          visited.push(next[i], true);
          result.push(next[i], true);
          queue.push_back({next[i], queue[head].depth + 1});
        }
      }
    }
    printList(output, setToVector(result));
    return true;
  }

  bool walletRankLess(const WalletRank& lhs, const WalletRank& rhs)
  {
    if (lhs.balance != rhs.balance) {
      return lhs.balance > rhs.balance;
    }
    return lhs.address < rhs.address;
  }

  bool topWalletsCommand(muraviev::CommandContext& context, const Tokens& tokens,
      std::ostream& output)
  {
    size_t top = 0;
    if (muraviev::countTokens(tokens) != 2 ||
        !muraviev::parsePositiveSize(muraviev::tokenAt(tokens, 1), top)) {
      return false;
    }
    std::vector< WalletRank > ranks;
    for (muraviev::WalletTree::const_iterator it = context.wallets().cbegin();
        it != context.wallets().cend(); ++it) {
      ranks.push_back({it->key, it->value.balance});
    }
    std::sort(ranks.begin(), ranks.end(), walletRankLess);
    if (ranks.empty()) {
      output << "<NOTHING FOUND>\n";
      return true;
    }
    const size_t count = top < ranks.size() ? top : ranks.size();
    for (size_t i = 0; i < count; ++i) {
      output << "<CASE: " << (i + 1) << ", ADDRESS: " << ranks[i].address
          << ", BALANCE: " << ranks[i].balance << ">\n";
    }
    return true;
  }

  std::string joinCycleKey(const std::vector< std::string >& cycle)
  {
    std::ostringstream output;
    for (size_t i = 0; i < cycle.size(); ++i) {
      if (i != 0) {
        output << '\n';
      }
      output << cycle[i];
    }
    return output.str();
  }

  std::vector< std::string > canonicalCycle(const std::vector< std::string >& path)
  {
    std::vector< std::string > cycle(path.begin(), path.end() - 1);
    size_t best = 0;
    for (size_t i = 1; i < cycle.size(); ++i) {
      for (size_t j = 0; j < cycle.size(); ++j) {
        const std::string& left = cycle[(i + j) % cycle.size()];
        const std::string& right = cycle[(best + j) % cycle.size()];
        if (left < right) {
          best = i;
          break;
        }
        if (right < left) {
          break;
        }
      }
    }
    std::vector< std::string > result;
    for (size_t i = 0; i < cycle.size(); ++i) {
      result.push_back(cycle[(best + i) % cycle.size()]);
    }
    result.push_back(result[0]);
    return result;
  }

  void searchCycles(const muraviev::CommandContext& context,
      const std::string& start, const std::string& current, size_t maxDepth,
      std::vector< std::string >& path, StringSet& used, StringSet& seen,
      std::vector< CycleInfo >& cycles)
  {
    if (path.size() > maxDepth) {
      return;
    }
    const std::vector< std::string > next = outgoing(context, current);
    for (size_t i = 0; i < next.size(); ++i) {
      if (next[i] == start && path.size() >= 2) {
        path.push_back(start);
        const std::vector< std::string > canonical = canonicalCycle(path);
        const std::string key = joinCycleKey(canonical);
        if (!seen.contains(key)) {
          seen.push(key, true);
          cycles.push_back({canonical, key});
        }
        path.pop_back();
      } else if (!used.contains(next[i]) && path.size() < maxDepth) {
        used.push(next[i], true);
        path.push_back(next[i]);
        searchCycles(context, start, next[i], maxDepth, path, used, seen, cycles);
        path.pop_back();
        used.drop(next[i]);
      }
    }
  }

  bool cycleLess(const CycleInfo& lhs, const CycleInfo& rhs)
  {
    if (lhs.path.size() != rhs.path.size()) {
      return lhs.path.size() < rhs.path.size();
    }
    return lhs.key < rhs.key;
  }

  bool detectCyclesCommand(muraviev::CommandContext& context, const Tokens& tokens,
      std::ostream& output)
  {
    size_t maxDepth = 0;
    size_t top = 0;
    if (muraviev::countTokens(tokens) != 3 ||
        !muraviev::parsePositiveSize(muraviev::tokenAt(tokens, 1), maxDepth) ||
        !muraviev::parsePositiveSize(muraviev::tokenAt(tokens, 2), top)) {
      return false;
    }
    StringSet seen;
    std::vector< CycleInfo > cycles;
    for (muraviev::WalletTree::const_iterator it = context.wallets().cbegin();
        it != context.wallets().cend(); ++it) {
      std::vector< std::string > path;
      StringSet used;
      path.push_back(it->key);
      used.push(it->key, true);
      searchCycles(context, it->key, it->key, maxDepth, path, used, seen, cycles);
    }
    std::sort(cycles.begin(), cycles.end(), cycleLess);
    if (cycles.empty()) {
      output << "<NOTHING FOUND>\n";
      return true;
    }
    const size_t count = top < cycles.size() ? top : cycles.size();
    for (size_t i = 0; i < count; ++i) {
      output << "<CASE: " << (i + 1) << ", LENGTH: "
          << (cycles[i].path.size() - 1) << ">\n<";
      for (size_t j = 0; j < cycles[i].path.size(); ++j) {
        if (j != 0) {
          output << " -> ";
        }
        output << cycles[i].path[j];
      }
      output << ">\n";
    }
    return true;
  }

  void addFlowEdge(std::vector< FlowEdge >& edges, const std::string& from,
      const std::string& to, long long amount, size_t order)
  {
    for (size_t i = 0; i < edges.size(); ++i) {
      if (edges[i].from == from && edges[i].to == to) {
        edges[i].amount += amount;
        return;
      }
    }
    edges.push_back({from, to, amount, order});
  }

  bool flowEdgeLess(const FlowEdge& lhs, const FlowEdge& rhs)
  {
    return lhs.order < rhs.order;
  }

  std::vector< FlowEdge > collectSchemeEdges(const std::vector< FlowState >& states,
      const std::string& target)
  {
    std::vector< FlowEdge > result;
    StringSet firstEdges;
    for (size_t i = 0; i < states.size(); ++i) {
      if (states[i].wallet != target || states[i].amount <= 0) {
        continue;
      }
      int current = static_cast< int >(i);
      while (current >= 0) {
        if (states[current].parent < 0) {
          const std::string key = states[current].edgeFrom + '\n' +
              states[current].edgeTo;
          if (!firstEdges.contains(key)) {
            firstEdges.push(key, true);
            addFlowEdge(result, states[current].edgeFrom, states[current].edgeTo,
                states[current].edgeAmount, states[current].edgeOrder);
          }
        } else {
          addFlowEdge(result, states[current].edgeFrom, states[current].edgeTo,
              states[i].amount, states[current].edgeOrder);
        }
        current = states[current].parent;
      }
    }
    std::sort(result.begin(), result.end(), flowEdgeLess);
    return result;
  }

  bool laundryLess(const LaundryInfo& lhs, const LaundryInfo& rhs)
  {
    if (lhs.score != rhs.score) {
      return lhs.score > rhs.score;
    }
    if (lhs.reached != rhs.reached) {
      return lhs.reached > rhs.reached;
    }
    if (lhs.branches != rhs.branches) {
      return lhs.branches > rhs.branches;
    }
    if (lhs.source != rhs.source) {
      return lhs.source < rhs.source;
    }
    return lhs.target < rhs.target;
  }

  void collectLaundryForSource(const muraviev::CommandContext& context,
      const std::string& source, size_t maxDepth, size_t minBranches,
      std::vector< LaundryInfo >& result)
  {
    std::vector< FlowState > states;
    for (muraviev::TransferLog::c_iter it = context.transferLog().begin();
        it != context.transferLog().end(); ++it) {
      if (it->fromAddress == source) {
        states.push_back({it->toAddress, it->toAddress, 1, it->amount, -1,
            it->fromAddress, it->toAddress, it->order, it->amount});
      } else {
        long long remaining = it->amount;
        const size_t statesCount = states.size();
        for (size_t i = 0; i < statesCount && remaining > 0; ++i) {
          if (states[i].wallet == it->fromAddress && states[i].amount > 0 &&
              states[i].depth < maxDepth) {
            const long long moved = states[i].amount < remaining ?
                states[i].amount : remaining;
            const std::string branch = states[i].branch;
            const size_t nextDepth = states[i].depth + 1;
            states[i].amount -= moved;
            remaining -= moved;
            states.push_back({it->toAddress, branch, nextDepth, moved,
                static_cast< int >(i), it->fromAddress, it->toAddress,
                it->order, moved});
          }
        }
      }
    }

    for (muraviev::WalletTree::const_iterator wallet = context.wallets().cbegin();
        wallet != context.wallets().cend(); ++wallet) {
      if (wallet->key == source) {
        continue;
      }
      StringSet branches;
      long long reached = 0;
      size_t depth = 0;
      for (size_t i = 0; i < states.size(); ++i) {
        if (states[i].wallet == wallet->key && states[i].amount > 0) {
          branches.push(states[i].branch, true);
          reached += states[i].amount;
          if (states[i].depth > depth) {
            depth = states[i].depth;
          }
        }
      }
      if (branches.size() >= minBranches) {
        const long long score = reached * static_cast< long long >(branches.size()) +
            static_cast< long long >(depth) * 4000;
        const std::vector< FlowEdge > edges = collectSchemeEdges(states, wallet->key);
        result.push_back({source, wallet->key, branches.size(), depth, reached,
            score, edges});
      }
    }
  }

  bool detectLaundryCommand(muraviev::CommandContext& context, const Tokens& tokens,
      std::ostream& output)
  {
    size_t maxDepth = 0;
    size_t minBranches = 0;
    size_t top = 0;
    if (muraviev::countTokens(tokens) != 4 ||
        !muraviev::parsePositiveSize(muraviev::tokenAt(tokens, 1), maxDepth) ||
        !muraviev::parsePositiveSize(muraviev::tokenAt(tokens, 2), minBranches) ||
        minBranches < 2 ||
        !muraviev::parsePositiveSize(muraviev::tokenAt(tokens, 3), top)) {
      return false;
    }
    std::vector< LaundryInfo > schemes;
    for (muraviev::WalletTree::const_iterator it = context.wallets().cbegin();
        it != context.wallets().cend(); ++it) {
      collectLaundryForSource(context, it->key, maxDepth, minBranches, schemes);
    }
    std::sort(schemes.begin(), schemes.end(), laundryLess);
    if (schemes.empty()) {
      output << "<NOTHING FOUND>\n";
      return true;
    }
    const size_t count = top < schemes.size() ? top : schemes.size();
    for (size_t i = 0; i < count; ++i) {
      output << "<CASE: " << (i + 1) << ", SOURCE: " << schemes[i].source
          << ", TARGET: " << schemes[i].target << ", SCORE: "
          << schemes[i].score << ">\n";
      output << "<BRANCHES: " << schemes[i].branches << ", DEPTH: "
          << schemes[i].depth << ", REACHED: " << schemes[i].reached << ">\n";
      for (size_t j = 0; j < schemes[i].edges.size(); ++j) {
        output << '<' << schemes[i].edges[j].from << " -> "
            << schemes[i].edges[j].to << " : " << schemes[i].edges[j].amount
            << ">\n";
      }
    }
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

  HelpTable createHelpTable()
  {
    HelpTable help;
    help.push("make-wallet", HelpInfo(
        "make-wallet <address> <label> <balance>",
        "Creates a wallet with a unique address and an initial balance.",
        "address   Unique wallet address.\n"
        "  label     Wallet name.\n"
        "  balance   Non-negative initial balance.",
        "make-wallet w1 main 1000",
        "(no output on success)",
        "Duplicate addresses and negative balances are rejected."));
    help.push("show-wallet", HelpInfo(
        "show-wallet <address>",
        "Displays wallet information, balance and transfer statistics.",
        "address   Address of an existing wallet.",
        "make-wallet w1 main 1000\n"
        "  show-wallet w1",
        "<ADDRESS: w1, LABEL: main, BALANCE: 1000, IN-COUNT: 0,\n"
        "  OUT-COUNT: 0, IN-SUM: 0, OUT-SUM: 0>",
        "A missing wallet is rejected."));
    help.push("drop-wallet", HelpInfo(
        "drop-wallet <address>",
        "Deletes an existing wallet without transfer history.",
        "address   Address of the wallet to delete.",
        "make-wallet w1 main 1000\n"
        "  drop-wallet w1",
        "(no output on success)",
        "A wallet with incoming or outgoing transfers cannot be deleted."));
    help.push("wallets", HelpInfo(
        "wallets",
        "Displays all wallet addresses in alphabetical order.",
        "",
        "make-wallet w2 second 500\n"
        "  make-wallet w1 first 1000\n"
        "  wallets",
        "<w1, w2>",
        "Prints <NOTHING FOUND> when there are no wallets."));
    help.push("make-transfer", HelpInfo(
        "make-transfer <id> <from> <to> <amount>",
        "Transfers funds between two existing wallets.",
        "id       Unique transfer ID.\n"
        "  from     Source wallet address.\n"
        "  to       Target wallet address.\n"
        "  amount   Positive transfer amount.",
        "make-wallet w1 source 1000\n"
        "  make-wallet w2 target 0\n"
        "  make-transfer t1 w1 w2 250",
        "(no output on success)",
        "Insufficient funds and non-positive amounts are rejected.\n"
        "  Arithmetic overflow prints <OVERFLOW ERROR>."));
    help.push("show-transfer", HelpInfo(
        "show-transfer <id>",
        "Displays information about one transfer.",
        "id   ID of an existing transfer.",
        "make-wallet w1 source 1000\n"
        "  make-wallet w2 target 0\n"
        "  make-transfer t1 w1 w2 250\n"
        "  show-transfer t1",
        "<ID: t1, FROM: w1, TO: w2, AMOUNT: 250>",
        "A missing transfer is rejected."));
    help.push("transfers", HelpInfo(
        "transfers <address>",
        "Displays IDs of all transfers related to a wallet.",
        "address   Address of an existing wallet.",
        "make-wallet w1 source 100\n"
        "  make-wallet w2 target 0\n"
        "  make-transfer t1 w1 w2 25\n"
        "  transfers w1",
        "<t1>",
        "Prints <NOTHING FOUND> when the wallet has no transfers."));
    help.push("path", HelpInfo(
        "path <from> <to>",
        "Finds a shortest directed transfer path between two wallets.",
        "from   Starting wallet address.\n"
        "  to     Target wallet address.",
        "make-wallet w1 source 100\n"
        "  make-wallet w2 middle 0\n"
        "  make-wallet w4 target 0\n"
        "  make-transfer t1 w1 w2 10\n"
        "  make-transfer t2 w2 w4 5\n"
        "  path w1 w4",
        "<w1 -> w2 -> w4>",
        "Prints <NOTHING FOUND> when no path exists."));
    help.push("related", HelpInfo(
        "related <address>",
        "Displays wallets directly connected by incoming and outgoing transfers.",
        "address   Address of an existing wallet.",
        "make-wallet w1 source 100\n"
        "  make-wallet w2 middle 0\n"
        "  make-wallet w4 target 0\n"
        "  make-transfer t1 w1 w2 10\n"
        "  make-transfer t2 w2 w4 5\n"
        "  related w2",
        "<IN: w1; OUT: w4>",
        "Prints <NOTHING FOUND> when there are no direct connections."));
    help.push("sinks", HelpInfo(
        "sinks <address> <max-depth>",
        "Displays wallets reachable within the specified transfer depth.",
        "address     Starting wallet address.\n"
        "  max-depth   Positive maximum path depth.",
        "make-wallet w1 source 100\n"
        "  make-wallet w2 first 0\n"
        "  make-wallet w3 second 0\n"
        "  make-wallet w4 target 0\n"
        "  make-transfer t1 w1 w2 10\n"
        "  make-transfer t2 w1 w3 10\n"
        "  make-transfer t3 w2 w4 5\n"
        "  sinks w1 2",
        "<w2, w3, w4>",
        "Results are unique and sorted by address."));
    help.push("top-wallets", HelpInfo(
        "top-wallets <top-k>",
        "Displays wallets with the highest balances.",
        "top-k   Positive maximum number of wallets to display.",
        "make-wallet w1 first 1000\n"
        "  make-wallet w2 second 500\n"
        "  make-wallet w3 third 100\n"
        "  top-wallets 2",
        "<CASE: 1, ADDRESS: w1, BALANCE: 1000>\n"
        "  <CASE: 2, ADDRESS: w2, BALANCE: 500>",
        "Equal balances are ordered by wallet address."));
    help.push("detect-cycles", HelpInfo(
        "detect-cycles <max-depth> <top-k>",
        "Finds unique directed transfer cycles.",
        "max-depth   Positive maximum cycle length.\n"
        "  top-k       Positive maximum number of results.",
        "make-wallet w1 first 100\n"
        "  make-wallet w2 second 0\n"
        "  make-wallet w3 third 0\n"
        "  make-transfer t1 w1 w2 10\n"
        "  make-transfer t2 w2 w3 5\n"
        "  make-transfer t3 w3 w1 1\n"
        "  detect-cycles 3 1",
        "<CASE: 1, LENGTH: 3>\n"
        "  <w1 -> w2 -> w3 -> w1>",
        "Prints <NOTHING FOUND> when no cycle is found."));
    help.push("detect-laundry", HelpInfo(
        "detect-laundry <max-depth> <min-branches> <top-k>",
        "Finds suspicious money flows that split into branches and reach\n"
        "  a common target.",
        "max-depth     Positive maximum transfer-chain depth.\n"
        "  min-branches  Minimum number of branches (at least 2).\n"
        "  top-k         Positive maximum number of results.",
        "make-wallet s source 100\n"
        "  make-wallet a1 branch1 0\n"
        "  make-wallet a2 branch2 0\n"
        "  make-wallet x middle 0\n"
        "  make-wallet t target 0\n"
        "  make-transfer e1 s a1 50\n"
        "  make-transfer e2 s a2 50\n"
        "  make-transfer e3 a1 x 10\n"
        "  make-transfer e4 a2 x 10\n"
        "  make-transfer e5 x t 20\n"
        "  detect-laundry 3 2 1",
        "<CASE: 1, SOURCE: s, TARGET: t, SCORE: 12040>\n"
        "  <BRANCHES: 2, DEPTH: 3, REACHED: 20>\n"
        "  <s -> a1 : 50>\n"
        "  <s -> a2 : 50>\n"
        "  <a1 -> x : 10>\n"
        "  <a2 -> x : 10>\n"
        "  <x -> t : 20>",
        "REACHED is the amount that reached the target.\n"
        "  Edge amounts show the recorded transfer history.\n"
        "  SCORE = REACHED * BRANCHES + DEPTH * 4000.\n"
        "  A higher score means a larger, deeper or more branched flow."));
    help.push("save", HelpInfo(
        "save <filename>",
        "Saves wallets, transfers and statistics to a file.",
        "filename   Name or path of the destination file.",
        "make-wallet w1 main 100\n"
        "  make-wallet w2 dst 0\n"
        "  make-transfer t1 w1 w2 40\n"
        "  save state.txt",
        "<OK>",
        "An unavailable destination file is rejected."));
    help.get("save").fileFormat =
        "WALLETS <wallet-count>\n"
        "  W <address> <label> <balance> <in-count> <out-count> <in-sum> <out-sum>\n"
        "  TRANSFERS <transfer-count>\n"
        "  T <id> <from> <to> <amount> <order>\n"
        "\n"
        "  Example file:\n"
        "  WALLETS 2\n"
        "  W w1 main 60 0 1 0 40\n"
        "  W w2 dst 40 1 0 40 0\n"
        "  TRANSFERS 1\n"
        "  T t1 w1 w2 40 1";
    help.push("load", HelpInfo(
        "load <filename>",
        "Loads wallets, transfers and statistics from a saved file.",
        "filename   Name or path of an existing state file.",
        "make-wallet w1 main 100\n"
        "  make-wallet w2 dst 0\n"
        "  make-transfer t1 w1 w2 40\n"
        "  save state.txt\n"
        "  load state.txt",
        "(no output on success)",
        "Invalid files are rejected without replacing the current state."));
    help.get("load").fileFormat =
        "WALLETS <wallet-count>\n"
        "  W <address> <label> <balance> <in-count> <out-count> <in-sum> <out-sum>\n"
        "  TRANSFERS <transfer-count>\n"
        "  T <id> <from> <to> <amount> <order>\n"
        "\n"
        "  Example file:\n"
        "  WALLETS 2\n"
        "  W w1 main 60 0 1 0 40\n"
        "  W w2 dst 40 1 0 40 0\n"
        "  TRANSFERS 1\n"
        "  T t1 w1 w2 40 1";
    return help;
  }

  void printDetailedHelp(std::ostream& output, const std::string& name,
      const HelpInfo& info)
  {
    std::string title = name;
    std::transform(title.begin(), title.end(), title.begin(), ::toupper);
    output << "=== " << title << " ===\n\n"
        << "USAGE:\n  " << info.usage << "\n\n"
        << "DESCRIPTION:\n  " << info.description << "\n\n";
    if (!info.parameters.empty()) {
      output << "PARAMETERS:\n  " << info.parameters << "\n\n";
    }
    output << "EXAMPLE:\n  " << info.example << "\n\n"
        << "OUTPUT:\n  " << info.result << "\n\n";
    if (!info.fileFormat.empty()) {
      output << "FILE FORMAT:\n  " << info.fileFormat << "\n\n";
    }
    output << "NOTES:\n  " << info.notes << '\n';
  }

  bool helpCommand(muraviev::CommandContext&, const Tokens& tokens,
      std::ostream& output)
  {
    const size_t tokenCount = muraviev::countTokens(tokens);
    if (tokenCount == 2) {
      const std::string& name = muraviev::tokenAt(tokens, 1);
      HelpTable help = createHelpTable();
      if (!help.contains(name)) {
        return false;
      }
      printDetailedHelp(output, name, help.get(name));
      return true;
    }
    if (tokenCount != 1) {
      return false;
    }
    output <<
        "=== AVAILABLE COMMANDS ===\n\n"
        "make-wallet <address> <label> <balance>\n"
        "  Create a wallet.\n\n"
        "show-wallet <address>\n"
        "  Show wallet information and statistics.\n\n"
        "drop-wallet <address>\n"
        "  Delete a wallet without transfer history.\n\n"
        "wallets\n"
        "  Show all wallet addresses.\n\n"
        "make-transfer <id> <from> <to> <amount>\n"
        "  Make a transfer between wallets.\n\n"
        "show-transfer <id>\n"
        "  Show transfer information.\n\n"
        "transfers <address>\n"
        "  Show all transfer IDs related to a wallet.\n\n"
        "path <from> <to>\n"
        "  Find a path between wallets.\n\n"
        "related <address>\n"
        "  Show directly related wallets.\n\n"
        "sinks <address> <max-depth>\n"
        "  Find reachable wallets.\n\n"
        "top-wallets <top-k>\n"
        "  Show wallets with the highest balances.\n\n"
        "detect-cycles <max-depth> <top-k>\n"
        "  Find transfer cycles.\n\n"
        "detect-laundry <max-depth> <min-branches> <top-k>\n"
        "  Find suspicious money flows.\n\n"
        "save <filename>\n"
        "  Save the current state.\n\n"
        "load <filename>\n"
        "  Load a saved state.\n\n"
        "Use help <command> for detailed information.\n";
    return true;
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
    commands.push("path", pathCommand);
    commands.push("related", relatedCommand);
    commands.push("sinks", sinksCommand);
    commands.push("top-wallets", topWalletsCommand);
    commands.push("detect-cycles", detectCyclesCommand);
    commands.push("detect-laundry", detectLaundryCommand);
    commands.push("save", saveCommand);
    commands.push("load", loadCommand);
    commands.push("help", helpCommand);
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
