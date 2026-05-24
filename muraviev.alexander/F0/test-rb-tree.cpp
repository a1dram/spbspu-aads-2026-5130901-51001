#include <boost/test/unit_test.hpp>

#include <stdexcept>
#include <string>

#include "rb-tree.hpp"

using TestTree = muraviev::RBTree< int, std::string, muraviev::Less< int > >;

BOOST_AUTO_TEST_CASE(test_rb_tree_empty_begin_end)
{
  TestTree tree;

  BOOST_TEST(tree.empty());
  BOOST_TEST(tree.size() == 0);
  BOOST_TEST(tree.begin() == tree.end());
  BOOST_TEST(tree.valid());
}

BOOST_AUTO_TEST_CASE(test_rb_tree_insert_get_replace_and_order)
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
  BOOST_TEST(tree.valid());

  const int keys[] = {1, 2, 3};
  size_t index = 0;
  for (TestTree::const_iterator it = tree.cbegin(); it != tree.cend(); ++it) {
    BOOST_REQUIRE(index < 3);
    BOOST_TEST(it->key == keys[index]);
    ++index;
  }
  BOOST_TEST(index == 3);
}

BOOST_AUTO_TEST_CASE(test_rb_tree_remove_leaf_one_child_two_children_and_root)
{
  TestTree tree;

  for (int i = 1; i <= 9; ++i) {
    tree.push(i, std::to_string(i));
    BOOST_TEST(tree.valid());
  }

  BOOST_TEST(tree.drop(9) == "9");
  BOOST_TEST(tree.valid());
  BOOST_TEST(tree.drop(8) == "8");
  BOOST_TEST(tree.valid());
  BOOST_TEST(tree.drop(4) == "4");
  BOOST_TEST(tree.valid());
  BOOST_TEST(tree.drop(5) == "5");
  BOOST_TEST(tree.valid());
  BOOST_CHECK_THROW(tree.drop(100), std::out_of_range);
}

BOOST_AUTO_TEST_CASE(test_rb_tree_copy_assignment_move_and_clear)
{
  TestTree tree;
  tree.push(2, "two");
  tree.push(1, "one");
  tree.push(3, "three");

  TestTree copy(tree);
  TestTree assigned;
  assigned = tree;
  TestTree moved(static_cast< TestTree&& >(tree));

  BOOST_TEST(copy.valid());
  BOOST_TEST(assigned.valid());
  BOOST_TEST(moved.valid());
  BOOST_TEST(copy.get(1) == "one");
  BOOST_TEST(assigned.get(3) == "three");
  BOOST_TEST(moved.size() == 3);

  assigned.clear();
  BOOST_TEST(assigned.empty());
  BOOST_TEST(assigned.valid());
}
