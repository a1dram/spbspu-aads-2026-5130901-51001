#include <boost/test/unit_test.hpp>

#include "parsing.hpp"

BOOST_AUTO_TEST_CASE(test_parsing_first_tokens)
{
  muraviev::TokenList tokens;
  BOOST_TEST(muraviev::splitStrictSpaces("make-wallet w1 main 100", tokens));
}
