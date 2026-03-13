#define main dont_use_main
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

BOOST_AUTO_TEST_CASE(test_list_insert_after_last)
{
  List< int > lst;
  lst.pushFront(1);
  lst.insert(lst.last(), 2);
  lst.insert(lst.last(), 3);

  int expected[3] = {1, 2, 3};
  size_t i = 0;
  for (List< int >::c_iter it = lst.begin(); it != lst.end(); ++it) {
    BOOST_TEST(*it == expected[i]);
    ++i;
  }
  BOOST_TEST(i == 3);
}

BOOST_AUTO_TEST_CASE(test_list_pop_front)
{
  List< int > lst;
  lst.pushFront(3);
  lst.pushFront(2);
  lst.pushFront(1);

  lst.popFront();

  int expected[2] = {2, 3};
  size_t i = 0;
  for (List< int >::c_iter it = lst.begin(); it != lst.end(); ++it) {
    BOOST_TEST(*it == expected[i]);
    ++i;
  }
  BOOST_TEST(i == 2);
}

BOOST_AUTO_TEST_CASE(test_list_pop_back)
{
  List< int > lst;
  lst.pushFront(3);
  lst.pushFront(2);
  lst.pushFront(1);

  lst.popBack();

  int expected[2] = {1, 2};
  size_t i = 0;
  for (List< int >::c_iter it = lst.begin(); it != lst.end(); ++it) {
    BOOST_TEST(*it == expected[i]);
    ++i;
  }
  BOOST_TEST(i == 2);
}

BOOST_AUTO_TEST_CASE(test_list_erase_middle)
{
  List< int > lst;
  lst.pushFront(3);
  lst.pushFront(2);
  lst.pushFront(1);

  List< int >::iter it = lst.begin();
  ++it;
  lst.erase(it);

  int expected[2] = {1, 3};
  size_t i = 0;
  for (List< int >::c_iter jt = lst.begin(); jt != lst.end(); ++jt) {
    BOOST_TEST(*jt == expected[i]);
    ++i;
  }
  BOOST_TEST(i == 2);
}

BOOST_AUTO_TEST_CASE(test_list_clear)
{
  List< int > lst;
  lst.pushFront(1);
  lst.pushFront(2);
  lst.pushFront(3);

  lst.clear();

  BOOST_TEST(lst.empty());
}

BOOST_AUTO_TEST_CASE(test_list_copy_constructor)
{
  List< int > a;
  a.pushFront(3);
  a.pushFront(2);
  a.pushFront(1);

  List< int > b(a);

  List< int >::c_iter it1 = a.begin();
  List< int >::c_iter it2 = b.begin();
  while (it1 != a.end() && it2 != b.end()) {
    BOOST_TEST(*it1 == *it2);
    ++it1;
    ++it2;
  }

  int count1 = 0;
  for (List< int >::c_iter it = a.begin(); it != a.end(); ++it) {
    ++count1;
  }

  int count2 = 0;
  for (List< int >::c_iter it = b.begin(); it != b.end(); ++it) {
    ++count2;
  }

  BOOST_TEST(count1 == count2);
}

BOOST_AUTO_TEST_CASE(test_list_iterators_on_loop)
{
  List< int > lst;
  lst.pushFront(3);
  lst.pushFront(2);
  lst.pushFront(1);

  int expected[3] = {1, 2, 3};
  size_t i = 0;
  for (List< int >::iter it = lst.begin(); it != lst.end(); ++it) {
    BOOST_TEST(*it == expected[i]);
    ++i;
  }
  BOOST_TEST(i == 3);
}

BOOST_AUTO_TEST_CASE(test_get_list_size)
{
  List< size_t > nums;
  nums.pushFront(3);
  nums.pushFront(2);
  nums.pushFront(1);

  size_t size = getListSize(nums);
  BOOST_TEST(size == 3);
}

BOOST_AUTO_TEST_CASE(test_has_index)
{
  List< size_t > nums;
  nums.pushFront(3);
  nums.pushFront(2);
  nums.pushFront(1);

  BOOST_TEST(hasIndex(nums, 0));
  BOOST_TEST(hasIndex(nums, 1));
  BOOST_TEST(hasIndex(nums, 2));
  BOOST_TEST(!hasIndex(nums, 3));
}

BOOST_AUTO_TEST_CASE(test_get_value_at)
{
  List< size_t > nums;
  nums.pushFront(3);
  nums.pushFront(2);
  nums.pushFront(1);

  BOOST_TEST(getValueAt(nums, 0) == 1);
  BOOST_TEST(getValueAt(nums, 1) == 2);
  BOOST_TEST(getValueAt(nums, 2) == 3);
}
