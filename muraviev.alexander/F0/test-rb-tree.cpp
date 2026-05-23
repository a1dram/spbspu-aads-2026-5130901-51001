#include <boost/test/unit_test.hpp>
#include "rb-tree.hpp"

BOOST_AUTO_TEST_CASE(test_rb_tree_less)
{
  muraviev::Less< int > less;
  BOOST_TEST(less(1, 2));
}
