#include <boost/test/unit_test.hpp>

#include "parsing.hpp"

BOOST_AUTO_TEST_CASE(test_parsing_first_tokens)
{
  muraviev::TokenList tokens;
  BOOST_TEST(muraviev::splitStrictSpaces("make-wallet w1 main 100", tokens));
  BOOST_TEST(muraviev::countTokens(tokens) == 4);
  BOOST_TEST(muraviev::tokenAt(tokens, 0) == "make-wallet");
}

BOOST_AUTO_TEST_CASE(test_parsing_strict_spaces)
{
  muraviev::TokenList tokens;
  BOOST_TEST(!muraviev::splitStrictSpaces(" make-wallet", tokens));
  BOOST_TEST(!muraviev::splitStrictSpaces("make-wallet ", tokens));
  BOOST_TEST(!muraviev::splitStrictSpaces("make-wallet  w1", tokens));
}

BOOST_AUTO_TEST_CASE(test_parsing_numbers)
{
  long long value = 0;
  size_t size = 0;
  BOOST_TEST(muraviev::parseNonNegativeLongLong("0", value));
  BOOST_TEST(muraviev::parsePositiveLongLong("12", value));
  BOOST_TEST(!muraviev::parsePositiveLongLong("0", value));
  BOOST_TEST(!muraviev::parseNonNegativeLongLong("-1", value));
  BOOST_TEST(muraviev::parsePositiveSize("3", size));
}
