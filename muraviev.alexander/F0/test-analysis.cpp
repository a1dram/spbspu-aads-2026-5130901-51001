#include <boost/test/unit_test.hpp>

#include <sstream>
#include <string>

#include "commands.hpp"

namespace
{
  std::string runAnalysis(const std::string& text)
  {
    muraviev::CommandContext context;
    std::istringstream input(text);
    std::ostringstream output;
    muraviev::executeCommands(input, output, context);
    return output.str();
  }
}

BOOST_AUTO_TEST_CASE(test_analysis_path_related_sinks_and_top)
{
  BOOST_TEST(runAnalysis(
      "make-wallet w1 a 100\nmake-wallet w2 b 0\nmake-wallet w3 c 0\nmake-wallet w4 d 0\n"
      "make-transfer t1 w1 w3 10\nmake-transfer t2 w1 w2 10\nmake-transfer t3 w2 w4 5\n"
      "path w1 w4\nrelated w2\nsinks w1 2\ntop-wallets 2\n") ==
      "<w1 -> w2 -> w4>\n<IN: w1; OUT: w4>\n<w2, w3, w4>\n"
      "<CASE: 1, ADDRESS: w1, BALANCE: 80>\n<CASE: 2, ADDRESS: w3, BALANCE: 10>\n");
}

BOOST_AUTO_TEST_CASE(test_analysis_path_no_path_and_invalid_cycles)
{
  BOOST_TEST(runAnalysis("make-wallet w1 a 1\nmake-wallet w2 b 1\npath w2 w1\ndetect-cycles 0 1\n") ==
      "<NOTHING FOUND>\n<INVALID COMMAND>\n");
}

