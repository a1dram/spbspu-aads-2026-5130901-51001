#include <boost/test/unit_test.hpp>
#include <cstdio>
#include <sstream>
#include <string>
#include "commands.hpp"

BOOST_AUTO_TEST_CASE(test_persistence_save_and_load_restore_wallet)
{
  muraviev::CommandContext context;
  std::istringstream input("make-wallet w1 main 100\nmake-wallet w2 dst 0\nmake-transfer t1 w1 w2 40\nsave test_f0_save.txt\nload test_f0_save.txt\nshow-wallet w2\n");
  std::ostringstream output;
  muraviev::executeCommands(input, output, context);
  BOOST_TEST(output.str().find("BALANCE: 40") != std::string::npos);
  std::remove("test_f0_save.txt");
}
