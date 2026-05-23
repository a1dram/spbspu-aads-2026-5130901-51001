#include <boost/test/unit_test.hpp>

#include <sstream>
#include <string>

#include "commands.hpp"
#include "parsing.hpp"

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

BOOST_AUTO_TEST_CASE(test_parsing_kept_from_parser_stage)
{
  muraviev::TokenList tokens; long long value = 0; size_t size = 0;
  BOOST_TEST(muraviev::splitStrictSpaces("make-wallet w1 main 100", tokens));
  BOOST_TEST(muraviev::countTokens(tokens) == 4);
  BOOST_TEST(muraviev::tokenAt(tokens, 0) == "make-wallet");
  BOOST_TEST(!muraviev::splitStrictSpaces("make-wallet  w1", tokens));
  BOOST_TEST(muraviev::parsePositiveLongLong("12", value));
  BOOST_TEST(!muraviev::parsePositiveLongLong("0", value));
  BOOST_TEST(muraviev::parsePositiveSize("3", size));
}

BOOST_AUTO_TEST_CASE(test_commands_make_wallet_show_wallet_and_duplicate)
{
  BOOST_TEST(runCommands("make-wallet w1 main 1000\nshow-wallet w1\nmake-wallet w1 other 1\n") ==
      "<ADDRESS: w1, LABEL: main, BALANCE: 1000, IN-COUNT: 0, OUT-COUNT: 0, IN-SUM: 0, OUT-SUM: 0>\n"
      "<INVALID COMMAND>\n");
}

BOOST_AUTO_TEST_CASE(test_commands_drop_wallet_and_wallets_sorted)
{
  BOOST_TEST(runCommands("make-wallet w2 b 0\nmake-wallet w1 a 0\nwallets\ndrop-wallet w1\nwallets\n") ==
      "<w1, w2>\n<w2>\n");
}

BOOST_AUTO_TEST_CASE(test_commands_transfer_updates_and_rejects_drop)
{
  BOOST_TEST(runCommands("make-wallet w1 src 1000\nmake-wallet w2 dst 0\nmake-transfer t1 w1 w2 250\nshow-wallet w1\nshow-wallet w2\nshow-transfer t1\ndrop-wallet w1\n") ==
      "<ADDRESS: w1, LABEL: src, BALANCE: 750, IN-COUNT: 0, OUT-COUNT: 1, IN-SUM: 0, OUT-SUM: 250>\n"
      "<ADDRESS: w2, LABEL: dst, BALANCE: 250, IN-COUNT: 1, OUT-COUNT: 0, IN-SUM: 250, OUT-SUM: 0>\n"
      "<ID: t1, FROM: w1, TO: w2, AMOUNT: 250>\n<INVALID COMMAND>\n");
}

BOOST_AUTO_TEST_CASE(test_commands_invalid_arguments_and_numbers)
{
  BOOST_TEST(runCommands("make-wallet w1 main -1\nmake-wallet w1 main 1 extra\nmake-wallet w1 main 1\nmake-transfer t1 w1 w1 1\nmake-transfer t2 w1 w1 abc\nwallets extra\n") ==
      "<INVALID COMMAND>\n<INVALID COMMAND>\n<INVALID COMMAND>\n<INVALID COMMAND>\n<INVALID COMMAND>\n");
}

BOOST_AUTO_TEST_CASE(test_commands_transfer_invalid_and_transfers_sorted)
{
  BOOST_TEST(runCommands("make-wallet w1 src 100\nmake-wallet w2 dst 0\nmake-transfer t2 w1 w2 10\nmake-transfer t1 w2 w1 5\nmake-transfer t3 w1 w2 1000\ntransfers w1\n") ==
      "<INVALID COMMAND>\n<t1, t2>\n");
}

