#include <boost/test/unit_test.hpp>

#include "stack.hpp"

using namespace muraviev;

BOOST_AUTO_TEST_CASE(test_stack_empty_after_default_constructor)
{
  Stack< int > st;
  BOOST_TEST(st.empty());
}

BOOST_AUTO_TEST_CASE(test_stack_push_top_drop)
{
  Stack< int > st;
  st.push(10);
  BOOST_TEST(!st.empty());
  BOOST_TEST(st.top() == 10);
  BOOST_TEST(st.drop() == 10);
  BOOST_TEST(st.empty());
}

BOOST_AUTO_TEST_CASE(test_stack_lifo_order)
{
  Stack< int > st;
  st.push(1);
  st.push(2);
  st.push(3);

  BOOST_TEST(st.drop() == 3);
  BOOST_TEST(st.drop() == 2);
  BOOST_TEST(st.drop() == 1);
}

BOOST_AUTO_TEST_CASE(test_stack_drop_from_empty_throws)
{
  Stack< int > st;
  BOOST_CHECK_THROW(st.drop(), std::logic_error);
}

BOOST_AUTO_TEST_CASE(test_stack_top_on_empty_throws)
{
  Stack< int > st;
  BOOST_CHECK_THROW(st.top(), std::logic_error);
}
