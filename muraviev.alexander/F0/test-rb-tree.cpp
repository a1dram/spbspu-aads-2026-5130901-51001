#include <boost/test/unit_test.hpp>
#include <stdexcept>
#include <string>
#include "rb-tree.hpp"

using TestTree = muraviev::RBTree< int, std::string, muraviev::Less< int > >;

BOOST_AUTO_TEST_CASE(test_rb_tree_insert_get_replace)
{
  TestTree tree;
  tree.push(3, "three");
  tree.push(1, "one");
  tree.push(3, "three-new");
  BOOST_TEST(tree.size() == 2);
  BOOST_TEST(tree.contains(3));
  BOOST_TEST(tree.get(3) == "three-new");
  BOOST_CHECK_THROW(tree.get(10), std::out_of_range);
  BOOST_TEST(tree.valid());
}
