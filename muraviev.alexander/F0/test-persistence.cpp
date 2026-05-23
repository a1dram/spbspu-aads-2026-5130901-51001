#include <boost/test/unit_test.hpp>

#include <cstdio>
#include <fstream>
#include <sstream>
#include <string>

#include "commands.hpp"

BOOST_AUTO_TEST_CASE(test_persistence_save_and_load_restore_wallet)
{
  const std::string filename = "test_f0_save.txt";
  muraviev::CommandContext context;
  std::istringstream input(
      "make-wallet w1 main 100\n"
      "make-wallet w2 dst 0\n"
      "make-transfer t1 w1 w2 40\n"
      "save test_f0_save.txt\n"
      "drop-wallet w2\n"
      "load test_f0_save.txt\n"
      "show-wallet w2\n");
  std::ostringstream output;

  muraviev::executeCommands(input, output, context);

  BOOST_TEST(output.str() ==
      "<OK>\n"
      "<INVALID COMMAND>\n"
      "<ADDRESS: w2, LABEL: dst, BALANCE: 40, IN-COUNT: 1, OUT-COUNT: 0, "
      "IN-SUM: 40, OUT-SUM: 0>\n");

  std::remove(filename.c_str());
}

BOOST_AUTO_TEST_CASE(test_persistence_missing_and_bad_file_keep_old_state)
{
  const std::string filename = "test_f0_bad.txt";
  {
    std::ofstream output(filename.c_str());
    output << "bad file\n";
  }

  muraviev::CommandContext context;
  std::istringstream input(
      "make-wallet w1 main 100\n"
      "load missing_f0_file.txt\n"
      "load test_f0_bad.txt\n"
      "show-wallet w1\n");
  std::ostringstream output;

  muraviev::executeCommands(input, output, context);

  BOOST_TEST(output.str() ==
      "<INVALID COMMAND>\n"
      "<INVALID COMMAND>\n"
      "<ADDRESS: w1, LABEL: main, BALANCE: 100, IN-COUNT: 0, OUT-COUNT: 0, "
      "IN-SUM: 0, OUT-SUM: 0>\n");

  std::remove(filename.c_str());
}
