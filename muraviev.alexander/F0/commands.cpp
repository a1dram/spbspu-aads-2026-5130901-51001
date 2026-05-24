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

  bool walletRankLess(const WalletRank& lhs, const WalletRank& rhs)
  {
    if (lhs.balance != rhs.balance) {
      return lhs.balance > rhs.balance;
    }
    return lhs.address < rhs.address;
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

  void collectFirstBranches(const muraviev::CommandContext& context,
      const std::string& source, std::vector< FlowState >& states)
  {
    for (muraviev::TransferLog::c_iter it = context.transferLog().begin();
        it != context.transferLog().end(); ++it) {
      if (it->fromAddress == source) {
        states.push_back({it->toAddress, it->toAddress, 1, it->amount, -1,
            it->fromAddress, it->toAddress, it->order, it->amount});
      }
    }
  }

  void propagateLaundryStates(const muraviev::CommandContext& context,
      size_t maxDepth, std::vector< FlowState >& states)
  {
    for (muraviev::TransferLog::c_iter it = context.transferLog().begin();
        it != context.transferLog().end(); ++it) {
      long long remaining = it->amount;
      const size_t statesCount = states.size();
      for (size_t i = 0; i < statesCount && remaining > 0; ++i) {
        if (states[i].wallet == it->fromAddress && states[i].depth < maxDepth) {
          const long long moved = states[i].amount < remaining ? states[i].amount : remaining;
          states[i].amount -= moved;
          remaining -= moved;
          states.push_back({it->toAddress, states[i].branch, states[i].depth + 1,
              moved, static_cast< int >(i), it->fromAddress, it->toAddress,
              it->order, moved});
        }
      }
    }
  }

  bool simpleLaundryLess(const LaundryInfo& lhs, const LaundryInfo& rhs)
  {
    if (lhs.score != rhs.score) { return lhs.score > rhs.score; }
    if (lhs.source != rhs.source) { return lhs.source < rhs.source; }
    return lhs.target < rhs.target;
  }

  void addSimpleLaundryEdge(std::vector< FlowEdge >& edges, const FlowState& state)
  {
    for (size_t i = 0; i < edges.size(); ++i) {
      if (edges[i].from == state.edgeFrom && edges[i].to == state.edgeTo) {
        edges[i].amount += state.edgeAmount;
        return;
      }
    }
    edges.push_back({state.edgeFrom, state.edgeTo, state.edgeAmount, state.edgeOrder});
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
      std::vector< FlowState > states;
      collectFirstBranches(context, it->key, states);
      propagateLaundryStates(context, maxDepth, states);
      for (muraviev::WalletTree::const_iterator wallet = context.wallets().cbegin();
          wallet != context.wallets().cend(); ++wallet) {
        if (wallet->key == it->key) { continue; }
        StringSet branches;
        long long reached = 0;
        size_t depth = 0;
        std::vector< FlowEdge > edges;
        for (size_t j = 0; j < states.size(); ++j) {
          if (states[j].wallet == wallet->key && states[j].amount > 0) {
            branches.push(states[j].branch, true);
            reached += states[j].amount;
            if (states[j].depth > depth) { depth = states[j].depth; }
            addSimpleLaundryEdge(edges, states[j]);
          }
        }
        if (branches.size() >= minBranches) {
          const long long score = reached * static_cast< long long >(branches.size()) +
              static_cast< long long >(depth) * 4000;
          schemes.push_back({it->key, wallet->key, branches.size(), depth, reached,
              score, edges});
        }
      }
    }
    std::sort(schemes.begin(), schemes.end(), simpleLaundryLess);
    if (schemes.empty()) { output << "<NOTHING FOUND>\n"; return true; }
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
    commands.push("save", saveCommand);
    commands.push("load", loadCommand);
    commands.push("detect-laundry", detectLaundryCommand);
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
