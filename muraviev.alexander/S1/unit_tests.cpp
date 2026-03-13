#define BOOST_TEST_MODULE S1
#include <boost/test/included/unit_test.hpp>

#include "list.hpp"
#include "sequence.hpp"
#include "read_seq.hpp"
#include "print_seq.hpp"

using namespace muraviev;

BOOST_AUTO_TEST_CASE(test_list_empty_after_default_constructor)
{
  List< int > lst;
  BOOST_TEST(lst.empty());
}

BOOST_AUTO_TEST_CASE(test_list_not_empty_after_push_front)
{
  List< int > lst;
  lst.pushFront(5);
  BOOST_TEST(!lst.empty());
}

BOOST_AUTO_TEST_CASE(test_list_push_front_order)
{
  List< int > lst;
  lst.pushFront(1);
  lst.pushFront(2);
  lst.pushFront(3);

  int expected[3] = {3, 2, 1};
  size_t i = 0;
  for (List< int >::c_iter it = lst.begin(); it != lst.end(); ++it) {
    BOOST_TEST(*it == expected[i]);
    ++i;
  }
  BOOST_TEST(i == 3);
}
