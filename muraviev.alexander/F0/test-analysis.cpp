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

BOOST_AUTO_TEST_CASE(test_analysis_detect_cycles_unique)
{
  BOOST_TEST(runAnalysis(
      "make-wallet w1 a 100\nmake-wallet w2 b 0\nmake-wallet w3 c 0\n"
      "make-transfer t1 w1 w2 10\nmake-transfer t2 w2 w3 5\nmake-transfer t3 w3 w1 1\n"
      "detect-cycles 3 5\n") ==
      "<CASE: 1, LENGTH: 3>\n<w1 -> w2 -> w3 -> w1>\n");
}

BOOST_AUTO_TEST_CASE(test_analysis_detect_laundry_example)
{
  BOOST_TEST(runAnalysis(
      "make-wallet w1 src 50000\nmake-wallet w2 a1 0\nmake-wallet w3 a2 0\nmake-wallet w4 a3 0\n"
      "make-wallet w5 m1 0\nmake-wallet w8 m2 0\nmake-wallet w9 dst 0\n"
      "make-transfer t1 w1 w2 5000\nmake-transfer t2 w1 w3 7000\nmake-transfer t3 w1 w4 6000\n"
      "make-transfer t4 w2 w5 5000\nmake-transfer t5 w3 w5 4000\nmake-transfer t6 w4 w8 6000\n"
      "make-transfer t7 w5 w9 9000\nmake-transfer t8 w8 w9 6000\ndetect-laundry 3 3 1\n") ==
      "<CASE: 1, SOURCE: w1, TARGET: w9, SCORE: 57000>\n"
      "<BRANCHES: 3, DEPTH: 3, REACHED: 15000>\n"
      "<w1 -> w2 : 5000>\n<w1 -> w3 : 7000>\n<w1 -> w4 : 6000>\n"
      "<w2 -> w5 : 5000>\n<w3 -> w5 : 4000>\n<w4 -> w8 : 6000>\n"
      "<w5 -> w9 : 9000>\n<w8 -> w9 : 6000>\n");
}

BOOST_AUTO_TEST_CASE(test_analysis_empty_results_are_reported)
{
  BOOST_TEST(runAnalysis("wallets\nmake-wallet w1 a 1\ntransfers w1\nrelated w1\nsinks w1 1\ndetect-cycles 3 1\ndetect-laundry 3 2 1\n") ==
      "<NOTHING FOUND>\n<NOTHING FOUND>\n<NOTHING FOUND>\n<NOTHING FOUND>\n<NOTHING FOUND>\n<NOTHING FOUND>\n");
}

