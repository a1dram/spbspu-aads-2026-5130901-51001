#include <boost/test/unit_test.hpp>

#include "expression.hpp"

using namespace muraviev;

BOOST_AUTO_TEST_CASE(test_expression_simple_addition)
{
  TokenArray tokens = splitTokens("2 + 3");
  Queue< std::string > postfix = infixToPostfix(tokens);
  BOOST_TEST(evaluatePostfix(postfix) == 5);
}

BOOST_AUTO_TEST_CASE(test_expression_with_parentheses)
{
  TokenArray tokens = splitTokens("( 2 + 3 ) * 4");
  Queue< std::string > postfix = infixToPostfix(tokens);
  BOOST_TEST(evaluatePostfix(postfix) == 20);
}

BOOST_AUTO_TEST_CASE(test_expression_multiple_binary_ops)
{
  TokenArray tokens = splitTokens("10 - 2 - 3");
  Queue< std::string > postfix = infixToPostfix(tokens);
  BOOST_TEST(evaluatePostfix(postfix) == 5);
}

BOOST_AUTO_TEST_CASE(test_expression_division)
{
  TokenArray tokens = splitTokens("8 / 2");
  Queue< std::string > postfix = infixToPostfix(tokens);
  BOOST_TEST(evaluatePostfix(postfix) == 4);
}

BOOST_AUTO_TEST_CASE(test_expression_mod)
{
  TokenArray tokens = splitTokens("8 % 3");
  Queue< std::string > postfix = infixToPostfix(tokens);
  BOOST_TEST(evaluatePostfix(postfix) == 2);
}

BOOST_AUTO_TEST_CASE(test_expression_power)
{
  TokenArray tokens = splitTokens("2 ** 3");
  Queue< std::string > postfix = infixToPostfix(tokens);
  BOOST_TEST(evaluatePostfix(postfix) == 8);
}

BOOST_AUTO_TEST_CASE(test_expression_parentheses_power)
{
  TokenArray tokens = splitTokens("( 2 + 3 ) ** 2");
  Queue< std::string > postfix = infixToPostfix(tokens);
  BOOST_TEST(evaluatePostfix(postfix) == 25);
}

BOOST_AUTO_TEST_CASE(test_expression_right_associative_power)
{
  TokenArray tokens = splitTokens("2 ** 3 ** 2");
  Queue< std::string > postfix = infixToPostfix(tokens);
  BOOST_TEST(evaluatePostfix(postfix) == 512);
}

BOOST_AUTO_TEST_CASE(test_expression_div_by_zero_throws)
{
  TokenArray tokens = splitTokens("5 / 0");
  Queue< std::string > postfix = infixToPostfix(tokens);
  BOOST_CHECK_THROW(evaluatePostfix(postfix), std::logic_error);
}

BOOST_AUTO_TEST_CASE(test_expression_mod_by_zero_throws)
{
  TokenArray tokens = splitTokens("5 % 0");
  Queue< std::string > postfix = infixToPostfix(tokens);
  BOOST_CHECK_THROW(evaluatePostfix(postfix), std::logic_error);
}

BOOST_AUTO_TEST_CASE(test_expression_negative_exponent_throws)
{
  TokenArray tokens = splitTokens("2 ** -1");
  Queue< std::string > postfix = infixToPostfix(tokens);
  BOOST_CHECK_THROW(evaluatePostfix(postfix), std::logic_error);
}

BOOST_AUTO_TEST_CASE(test_expression_unknown_token_throws)
{
  TokenArray tokens = splitTokens("2 & 3");
  BOOST_CHECK_THROW(infixToPostfix(tokens), std::logic_error);
}

BOOST_AUTO_TEST_CASE(test_expression_extra_close_parenthesis_throws)
{
  TokenArray tokens = splitTokens("2 + 3 )");
  BOOST_CHECK_THROW(infixToPostfix(tokens), std::logic_error);
}

BOOST_AUTO_TEST_CASE(test_expression_invalid_expression_throws)
{
  TokenArray tokens = splitTokens("2 3 +");
  BOOST_CHECK_THROW(infixToPostfix(tokens), std::logic_error);
}
