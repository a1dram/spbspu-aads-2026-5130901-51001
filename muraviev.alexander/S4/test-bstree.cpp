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
