#include <boost/test/unit_test.hpp>

#include <sstream>
#include <string>

#include "commands.hpp"

namespace
{
  std::string runCommands(const std::string& text)
  {
    muraviev::CommandContext context;
    std::istringstream input(text);
    std::ostringstream output;
    muraviev::executeCommands(input, output, context);
    return output.str();
  }
}

BOOST_AUTO_TEST_CASE(test_commands_make_wallet_show_wallet_and_duplicate)
{
  BOOST_TEST(runCommands(
      "make-wallet w1 main 1000\n"
      "show-wallet w1\n"
      "make-wallet w1 other 1\n") ==
      "<ADDRESS: w1, LABEL: main, BALANCE: 1000, IN-COUNT: 0, "
      "OUT-COUNT: 0, IN-SUM: 0, OUT-SUM: 0>\n"
      "<INVALID COMMAND>\n");
}

BOOST_AUTO_TEST_CASE(test_commands_drop_wallet_and_wallets_sorted)
{
  BOOST_TEST(runCommands(
      "make-wallet w2 b 0\n"
      "make-wallet w1 a 0\n"
      "wallets\n"
      "drop-wallet w1\n"
      "wallets\n") ==
      "<w1, w2>\n"
      "<w2>\n");
}

BOOST_AUTO_TEST_CASE(test_commands_transfer_updates_and_rejects_drop)
{
  BOOST_TEST(runCommands(
      "make-wallet w1 src 1000\n"
      "make-wallet w2 dst 0\n"
      "make-transfer t1 w1 w2 250\n"
      "show-wallet w1\n"
      "show-wallet w2\n"
      "show-transfer t1\n"
      "drop-wallet w1\n") ==
      "<ADDRESS: w1, LABEL: src, BALANCE: 750, IN-COUNT: 0, "
      "OUT-COUNT: 1, IN-SUM: 0, OUT-SUM: 250>\n"
      "<ADDRESS: w2, LABEL: dst, BALANCE: 250, IN-COUNT: 1, "
      "OUT-COUNT: 0, IN-SUM: 250, OUT-SUM: 0>\n"
      "<ID: t1, FROM: w1, TO: w2, AMOUNT: 250>\n"
      "<INVALID COMMAND>\n");
}

BOOST_AUTO_TEST_CASE(test_commands_transfer_invalid_and_transfers_sorted)
{
  BOOST_TEST(runCommands(
      "make-wallet w1 src 100\n"
      "make-wallet w2 dst 0\n"
      "make-transfer t2 w1 w2 10\n"
      "make-transfer t1 w2 w1 5\n"
      "make-transfer t3 w1 w2 1000\n"
      "transfers w1\n") ==
      "<INVALID COMMAND>\n"
      "<t1, t2>\n");
}

BOOST_AUTO_TEST_CASE(test_commands_invalid_arguments_and_numbers)
{
  BOOST_TEST(runCommands(
      "make-wallet w1 main -1\n"
      "make-wallet w1 main 1 extra\n"
      "make-wallet w1 main 1\n"
      "make-transfer t1 w1 w1 1\n"
      "make-transfer t2 w1 w1 abc\n"
      "wallets extra\n") ==
      "<INVALID COMMAND>\n"
      "<INVALID COMMAND>\n"
      "<INVALID COMMAND>\n"
      "<INVALID COMMAND>\n"
      "<INVALID COMMAND>\n");
}

BOOST_AUTO_TEST_CASE(test_commands_transfer_rejects_balance_overflow_atomically)
{
  BOOST_TEST(runCommands(
      "make-wallet src s 9223372036854775807\n"
      "make-wallet dst d 9223372036854775800\n"
      "make-transfer t1 src dst 10\n"
      "show-wallet src\n"
      "show-wallet dst\n"
      "top-wallets 2\n") ==
      "<OVERFLOW ERROR>\n"
      "<ADDRESS: src, LABEL: s, BALANCE: 9223372036854775807, IN-COUNT: 0, "
      "OUT-COUNT: 0, IN-SUM: 0, OUT-SUM: 0>\n"
      "<ADDRESS: dst, LABEL: d, BALANCE: 9223372036854775800, IN-COUNT: 0, "
      "OUT-COUNT: 0, IN-SUM: 0, OUT-SUM: 0>\n"
      "<CASE: 1, ADDRESS: src, BALANCE: 9223372036854775807>\n"
      "<CASE: 2, ADDRESS: dst, BALANCE: 9223372036854775800>\n");
}

BOOST_AUTO_TEST_CASE(test_commands_transfer_rejects_statistics_overflow_atomically)
{
  BOOST_TEST(runCommands(
      "make-wallet a A 9223372036854775807\n"
      "make-wallet b B 0\n"
      "make-wallet c C 9223372036854775807\n"
      "make-transfer t1 a b 9223372036854775807\n"
      "make-transfer t2 c b 1\n"
      "show-wallet b\n"
      "show-wallet c\n") ==
      "<OVERFLOW ERROR>\n"
      "<ADDRESS: b, LABEL: B, BALANCE: 9223372036854775807, IN-COUNT: 1, "
      "OUT-COUNT: 0, IN-SUM: 9223372036854775807, OUT-SUM: 0>\n"
      "<ADDRESS: c, LABEL: C, BALANCE: 9223372036854775807, IN-COUNT: 0, "
      "OUT-COUNT: 0, IN-SUM: 0, OUT-SUM: 0>\n");
}

BOOST_AUTO_TEST_CASE(test_commands_help)
{
  BOOST_TEST(runCommands("help\n") ==
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
      "Use help <command> for detailed information.\n");
}

BOOST_AUTO_TEST_CASE(test_commands_detailed_help)
{
  BOOST_TEST(runCommands("help detect-laundry\n") ==
      "=== DETECT-LAUNDRY ===\n\n"
      "USAGE:\n"
      "  detect-laundry <max-depth> <min-branches> <top-k>\n\n"
      "DESCRIPTION:\n"
      "  Finds suspicious money flows that split into branches and reach\n"
      "  a common target.\n\n"
      "PARAMETERS:\n"
      "  max-depth     Positive maximum transfer-chain depth.\n"
      "  min-branches  Minimum number of branches (at least 2).\n"
      "  top-k         Positive maximum number of results.\n\n"
      "EXAMPLE:\n"
      "  make-wallet s source 100\n"
      "  make-wallet a1 branch1 0\n"
      "  make-wallet a2 branch2 0\n"
      "  make-wallet x middle 0\n"
      "  make-wallet t target 0\n"
      "  make-transfer e1 s a1 50\n"
      "  make-transfer e2 s a2 50\n"
      "  make-transfer e3 a1 x 10\n"
      "  make-transfer e4 a2 x 10\n"
      "  make-transfer e5 x t 20\n"
      "  detect-laundry 3 2 1\n\n"
      "OUTPUT:\n"
      "  <CASE: 1, SOURCE: s, TARGET: t, SCORE: 12040>\n"
      "  <BRANCHES: 2, DEPTH: 3, REACHED: 20>\n"
      "  <s -> a1 : 50>\n"
      "  <s -> a2 : 50>\n"
      "  <a1 -> x : 10>\n"
      "  <a2 -> x : 10>\n"
      "  <x -> t : 20>\n\n"
      "NOTES:\n"
      "  REACHED is the amount that reached the target.\n"
      "  Edge amounts show the recorded transfer history.\n"
      "  SCORE = REACHED * BRANCHES + DEPTH * 4000.\n"
      "  A higher score means a larger, deeper or more branched flow.\n");
}

BOOST_AUTO_TEST_CASE(test_commands_detailed_help_rejects_unknown_commands)
{
  BOOST_TEST(runCommands(
      "help help\n"
      "help unknown\n"
      "help make-wallet extra\n") ==
      "<INVALID COMMAND>\n"
      "<INVALID COMMAND>\n"
      "<INVALID COMMAND>\n");
}

BOOST_AUTO_TEST_CASE(test_commands_detailed_help_exists_for_every_command)
{
  const std::string output = runCommands(
      "help make-wallet\n"
      "help show-wallet\n"
      "help drop-wallet\n"
      "help wallets\n"
      "help make-transfer\n"
      "help show-transfer\n"
      "help transfers\n"
      "help path\n"
      "help related\n"
      "help sinks\n"
      "help top-wallets\n"
      "help detect-cycles\n"
      "help detect-laundry\n"
      "help save\n"
      "help load\n");
  BOOST_TEST(output.find("<INVALID COMMAND>") == std::string::npos);
  BOOST_TEST(output.find("=== MAKE-WALLET ===") != std::string::npos);
  BOOST_TEST(output.find("=== LOAD ===") != std::string::npos);
}

BOOST_AUTO_TEST_CASE(test_commands_help_omits_empty_parameters_section)
{
  const std::string output = runCommands("help wallets\n");
  BOOST_TEST(output.find("PARAMETERS:") == std::string::npos);
  BOOST_TEST(output.find("(none)") == std::string::npos);
  BOOST_TEST(output.find(
      "EXAMPLE:\n  make-wallet w2 second 500\n"
      "  make-wallet w1 first 1000\n"
      "  wallets\n") != std::string::npos);
}

BOOST_AUTO_TEST_CASE(test_commands_save_and_load_help_show_file_format)
{
  const std::string output = runCommands(
      "help save\n"
      "help load\n");
  BOOST_TEST(output.find("WALLETS <wallet-count>") != std::string::npos);
  BOOST_TEST(output.find(
      "W <address> <label> <balance> <in-count> <out-count> <in-sum> <out-sum>") !=
      std::string::npos);
  BOOST_TEST(output.find("TRANSFERS <transfer-count>") != std::string::npos);
  BOOST_TEST(output.find("T <id> <from> <to> <amount> <order>") !=
      std::string::npos);
  BOOST_TEST(output.find("T t1 w1 w2 40 1") != std::string::npos);
  BOOST_TEST(output.find("FILE FORMAT:") != std::string::npos);
  BOOST_TEST(output.find("Example file:") != std::string::npos);
}
