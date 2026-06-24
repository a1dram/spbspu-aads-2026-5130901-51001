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
