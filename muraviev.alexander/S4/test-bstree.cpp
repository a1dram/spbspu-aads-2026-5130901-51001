#include <boost/test/unit_test.hpp>

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
