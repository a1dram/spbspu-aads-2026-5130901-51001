#include <boost/test/unit_test.hpp>

#include <stdexcept>
#include <string>

#include "dataset.hpp"
#include "bstree.hpp"

using TestTree = muraviev::BSTree< int, std::string, muraviev::Less< int > >;

BOOST_AUTO_TEST_CASE(test_bstree_empty_begin_end_and_height)
{
  TestTree tree;

  BOOST_TEST(tree.empty());
  BOOST_TEST(tree.size() == 0);
  BOOST_TEST(tree.begin() == tree.end());
  BOOST_TEST(tree.height() == 0);
  BOOST_TEST(tree.height(tree.cend()) == 0);
}

BOOST_AUTO_TEST_CASE(test_bstree_push_get_replace_and_iteration)
{
  TestTree tree;

  tree.push(3, "three");
  tree.push(1, "one");
  tree.push(2, "two");
  tree.push(3, "three-new");

  BOOST_TEST(tree.size() == 3);
  BOOST_TEST(tree.contains(3));
  BOOST_TEST(tree.get(3) == "three-new");
  BOOST_CHECK_THROW(tree.get(10), std::out_of_range);

  int expected = 1;
  for (TestTree::const_iterator it = tree.cbegin(); it != tree.cend(); ++it) {
    BOOST_TEST(it->key == expected);
    ++expected;
  }
  BOOST_TEST(expected == 4);
}

BOOST_AUTO_TEST_CASE(test_bstree_drop_leaf_one_child_and_two_children)
{
  TestTree tree;

  tree.push(4, "four");
  tree.push(2, "two");
  tree.push(6, "six");
  tree.push(1, "one");
  tree.push(3, "three");
  tree.push(5, "five");
  tree.push(7, "seven");

  BOOST_TEST(tree.drop(1) == "one");
  BOOST_TEST(tree.drop(7) == "seven");
  BOOST_TEST(tree.drop(2) == "two");
  BOOST_TEST(tree.drop(4) == "four");
  BOOST_CHECK_THROW(tree.drop(100), std::out_of_range);

  const int keys[] = {3, 5, 6};
  size_t index = 0;
  for (TestTree::const_iterator it = tree.cbegin(); it != tree.cend(); ++it) {
    BOOST_REQUIRE(index < 3);
    BOOST_TEST(it->key == keys[index]);
    ++index;
  }
  BOOST_TEST(index == 3);
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

BOOST_AUTO_TEST_CASE(test_bstree_rotations_update_root_and_keep_order)
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

  TestTree largeLeft;
  largeLeft.push(1, "one");
  largeLeft.push(3, "three");
  largeLeft.push(2, "two");
  TestTree::const_iterator largeLeftIt = largeLeft.cbegin();
  ++largeLeftIt;
  largeLeft.rotateLargeLeft(largeLeftIt);
  BOOST_TEST(largeLeft.height() == 2);

  TestTree largeRight;
  largeRight.push(3, "three");
  largeRight.push(1, "one");
  largeRight.push(2, "two");
  TestTree::const_iterator largeRightIt = largeRight.cbegin();
  ++largeRightIt;
  largeRight.rotateLargeRight(largeRightIt);
  BOOST_TEST(largeRight.height() == 2);
}
