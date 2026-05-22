#include <boost/test/unit_test.hpp>

#include <stdexcept>
#include <string>

#include "dataset.hpp"
#include "bstree.hpp"

using TestTree = muraviev::BSTree< int, std::string, muraviev::Less< int > >;

BOOST_AUTO_TEST_CASE(test_bstree_empty_tree)
{
  TestTree tree;

  BOOST_TEST(tree.empty());
  BOOST_TEST(tree.size() == 0);
  BOOST_TEST(tree.cbegin() == tree.cend());
}

BOOST_AUTO_TEST_CASE(test_bstree_push_get_and_replace)
{
  TestTree tree;

  tree.push(3, "three");
  tree.push(1, "one");
  tree.push(3, "three-new");

  BOOST_TEST(tree.size() == 2);
  BOOST_TEST(tree.contains(1));
  BOOST_TEST(tree.get(3) == "three-new");
  BOOST_CHECK_THROW(tree.get(10), std::out_of_range);
}

BOOST_AUTO_TEST_CASE(test_bstree_drops_leaf_and_one_child)
{
  TestTree tree;

  tree.push(2, "two");
  tree.push(1, "one");
  tree.push(3, "three");

  BOOST_TEST(tree.drop(1) == "one");
  BOOST_TEST(tree.drop(2) == "two");
  BOOST_TEST(tree.size() == 1);
  BOOST_CHECK_THROW(tree.drop(100), std::out_of_range);
}

BOOST_AUTO_TEST_CASE(test_bstree_drops_node_with_two_children)
{
  TestTree tree;

  tree.push(4, "four");
  tree.push(2, "two");
  tree.push(6, "six");
  tree.push(1, "one");
  tree.push(3, "three");
  tree.push(5, "five");
  tree.push(7, "seven");

  BOOST_TEST(tree.drop(4) == "four");
  BOOST_TEST(tree.size() == 6);
  BOOST_TEST(!tree.contains(4));
  BOOST_TEST(tree.contains(5));
}

BOOST_AUTO_TEST_CASE(test_bstree_copy_and_clear)
{
  TestTree tree;

  tree.push(2, "two");
  tree.push(1, "one");
  tree.push(3, "three");

  TestTree copy(tree);
  tree.clear();

  BOOST_TEST(tree.empty());
  BOOST_TEST(copy.size() == 3);
  BOOST_TEST(copy.get(1) == "one");
  BOOST_TEST(copy.get(2) == "two");
  BOOST_TEST(copy.get(3) == "three");
}

BOOST_AUTO_TEST_CASE(test_bstree_rotations_update_height)
{
  TestTree left;
  left.push(1, "one");
  left.push(2, "two");
  TestTree::const_iterator leftIt = left.cbegin();
  ++leftIt;
  left.rotateLeft(leftIt);
  BOOST_TEST(left.height() == 2);

  TestTree right;
  right.push(2, "two");
  right.push(1, "one");
  right.rotateRight(right.cbegin());
  BOOST_TEST(right.height() == 2);
}
