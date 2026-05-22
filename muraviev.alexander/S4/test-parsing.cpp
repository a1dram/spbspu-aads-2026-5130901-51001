#include <boost/test/unit_test.hpp>

#include <limits>
#include <string>

#include "parsing.hpp"

namespace
{
  std::string tokenAt(const muraviev::TokenList& tokens, size_t index)
  {
    size_t current = 0;
    for (muraviev::TokenList::c_iter it = tokens.begin(); it != tokens.end(); ++it) {
      if (current == index) {
        return *it;
      }
      ++current;
    }
    return "";
  }
}

BOOST_AUTO_TEST_CASE(test_parsing_splits_strict_spaces)
{
  muraviev::TokenList tokens;

  BOOST_TEST(muraviev::splitStrictSpaces("first 1 name", tokens));
  BOOST_TEST(muraviev::countTokens(tokens) == 3);
  BOOST_TEST(tokenAt(tokens, 0) == "first");
  BOOST_TEST(tokenAt(tokens, 1) == "1");
  BOOST_TEST(tokenAt(tokens, 2) == "name");

  BOOST_TEST(!muraviev::splitStrictSpaces("first  1", tokens));
  BOOST_TEST(!muraviev::splitStrictSpaces(" first 1", tokens));
  BOOST_TEST(!muraviev::splitStrictSpaces("first 1 ", tokens));
}

BOOST_AUTO_TEST_CASE(test_parsing_int)
{
  int value = 0;

  BOOST_TEST(muraviev::parseInt("0", value));
  BOOST_TEST(value == 0);
  BOOST_TEST(muraviev::parseInt("-42", value));
  BOOST_TEST(value == -42);
  BOOST_TEST(muraviev::parseInt("2147483647", value));
  BOOST_TEST(value == std::numeric_limits< int >::max());
  BOOST_TEST(muraviev::parseInt("-2147483648", value));
  BOOST_TEST(value == std::numeric_limits< int >::min());

  BOOST_TEST(!muraviev::parseInt("", value));
  BOOST_TEST(!muraviev::parseInt("-", value));
  BOOST_TEST(!muraviev::parseInt("12x", value));
  BOOST_TEST(!muraviev::parseInt("2147483648", value));
  BOOST_TEST(!muraviev::parseInt("-2147483649", value));
}
