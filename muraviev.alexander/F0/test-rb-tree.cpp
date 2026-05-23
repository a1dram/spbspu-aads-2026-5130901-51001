#include <boost/test/unit_test.hpp>
#include <stdexcept>
#include <string>
#include "rb-tree.hpp"
using TestTree = muraviev::RBTree< int, std::string, muraviev::Less< int > >;

BOOST_AUTO_TEST_CASE(test_rb_tree_less_empty)
{
  muraviev::Less< int > less; BOOST_TEST(less(1, 2));
  TestTree tree; BOOST_TEST(tree.empty()); BOOST_TEST(tree.size() == 0); BOOST_TEST(tree.valid());
}

BOOST_AUTO_TEST_CASE(test_rb_tree_insert_get_replace)
{
  TestTree tree; tree.push(3, "three"); tree.push(1, "one"); tree.push(3, "three-new");
  BOOST_TEST(tree.size() == 2); BOOST_TEST(tree.contains(3)); BOOST_TEST(tree.get(3) == "three-new");
  BOOST_CHECK_THROW(tree.get(10), std::out_of_range); BOOST_TEST(tree.valid());
}

BOOST_AUTO_TEST_CASE(test_rb_tree_clear)
{
  TestTree tree; tree.push(2, "two"); tree.push(1, "one"); tree.clear();
  BOOST_TEST(tree.empty()); BOOST_TEST(tree.valid());
}

BOOST_AUTO_TEST_CASE(test_rb_tree_order_iteration)
{
  TestTree tree; tree.push(3, "three"); tree.push(1, "one"); tree.push(2, "two"); int expected = 1;
  for (TestTree::iterator it = tree.begin(); it != tree.end(); ++it) { BOOST_TEST(it->key == expected); ++expected; }
  BOOST_TEST(expected == 4);
}

BOOST_AUTO_TEST_CASE(test_rb_tree_insert_many_needs_rotations)
{
  TestTree tree; for (int i = 1; i <= 20; ++i) { tree.push(i, std::to_string(i)); BOOST_TEST(tree.valid()); }
  BOOST_TEST(tree.size() == 20);
}

BOOST_AUTO_TEST_CASE(test_rb_tree_remove_leaf)
{
  TestTree tree; for (int i = 1; i <= 4; ++i) { tree.push(i, std::to_string(i)); }
  BOOST_TEST(tree.drop(4) == "4"); BOOST_CHECK_THROW(tree.drop(2), std::out_of_range);
}
