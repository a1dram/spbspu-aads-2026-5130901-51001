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

