#define BOOST_TEST_MODULE S1
#include <boost/test/included/unit_test.hpp>

#include "list.hpp"
#include "sequence.hpp"
#include "read_seq.hpp"
#include "print_seq.hpp"

using namespace muraviev;

BOOST_AUTO_TEST_CASE(test_list_is_empty)
{
  List< int > l;
  BOOST_TEST(l.empty());
}

BOOST_AUTO_TEST_CASE(test_list_push_front)
{
  List< int > l;
  l.pushFront(10);
  BOOST_TEST(!l.empty());
  BOOST_TEST(*l.begin() == 10);
}

BOOST_AUTO_TEST_CASE(test_list_insert)
{
  List< int > l;
  l.pushFront(1);
  l.insert(l.last(), 2);

  List< int >::iter it = l.begin();
  BOOST_TEST(*it == 1);
  ++it;
  BOOST_TEST(*it == 2);
}

BOOST_AUTO_TEST_CASE(test_list_pop_front)
{
  List< int > l;
  l.pushFront(2);
  l.pushFront(1);
  l.popFront();

  BOOST_TEST(*l.begin() == 2);
}

BOOST_AUTO_TEST_CASE(test_list_pop_back)
{
  List< int > l;
  l.pushFront(2);
  l.pushFront(1);
  l.popBack();

  List< int >::iter it = l.begin();
  BOOST_TEST(*it == 1);
  ++it;
  BOOST_TEST(it == l.end());
}

BOOST_AUTO_TEST_CASE(test_list_erase)
{
  List< int > l;
  l.pushFront(3);
  l.pushFront(2);
  l.pushFront(1);

  List< int >::iter it = l.begin();
  ++it;
  l.erase(it);

  it = l.begin();
  BOOST_TEST(*it == 1);
  ++it;
  BOOST_TEST(*it == 3);
}

BOOST_AUTO_TEST_CASE(test_list_clear)
{
  List< int > l;
  l.pushFront(1);
  l.clear();
  BOOST_TEST(l.empty());
}

BOOST_AUTO_TEST_CASE(test_list_copy_constructor)
{
  List< int > l1;
  l1.pushFront(1);
  l1.pushFront(2);

  List< int > l2(l1);
  List< int >::iter it = l2.begin();
  BOOST_TEST(*it == 2);
  ++it;
  BOOST_TEST(*it == 1);
}

BOOST_AUTO_TEST_CASE(test_list_iterators)
{
  List< int > l;
  l.pushFront(1);
  l.pushFront(2);
  int count = 0;
  for (List< int >::iter it = l.begin(); it != l.end(); ++it) {
    count++;
  }
  BOOST_TEST(count == 2);
}

BOOST_AUTO_TEST_CASE(test_print_get_list_size)
{
  List< size_t > nums;
  nums.pushFront(99);
  nums.pushFront(88);
  BOOST_TEST(getListSize(nums) == 2);
}

BOOST_AUTO_TEST_CASE(test_print_has_index)
{
  List< size_t > nums;
  nums.pushFront(5);
  nums.pushFront(4);
  BOOST_TEST(hasIndex(nums, 0));
  BOOST_TEST(hasIndex(nums, 1));
  BOOST_TEST(!hasIndex(nums, 2));
}

BOOST_AUTO_TEST_CASE(test_print_get_value_at)
{
  List< size_t > nums;
  nums.pushFront(42);
  nums.pushFront(24);
  BOOST_TEST(getValueAt(nums, 0) == 24);
  BOOST_TEST(getValueAt(nums, 1) == 42);
}

BOOST_AUTO_TEST_CASE(test_new_list_is_empty)
{
  List< int > l;
  BOOST_TEST(l.empty());
}

BOOST_AUTO_TEST_CASE(test_push_and_clear)
{
  List< int > l;
  l.pushFront(10);
  l.pushFront(20);
  BOOST_TEST(!l.empty());

  l.clear();
  BOOST_TEST(l.empty());
}

BOOST_AUTO_TEST_CASE(test_iterator_prefix_increment)
{
  List< int > l;
  l.pushFront(30);
  l.pushFront(20);
  l.pushFront(10);

  List< int >::iter it = l.begin();
  BOOST_TEST(*it == 10);
  ++it;
  BOOST_TEST(*it == 20);
  ++it;
  BOOST_TEST(*it == 30);
  ++it;

  BOOST_TEST(it == l.end());
}

BOOST_AUTO_TEST_CASE(test_copy_assignment_operator)
{
  List< int > l1;
  l1.pushFront(5);
  l1.pushFront(15);

  List< int > l2;
  l2 = l1;

  List< int >::iter it = l2.begin();
  BOOST_TEST(*it == 15);
  ++it;
  BOOST_TEST(*it == 5);
}

BOOST_AUTO_TEST_CASE(test_pop_front_till_empty)
{
  List< int > l;
  l.pushFront(100);
  l.pushFront(200);

  l.popFront();
  BOOST_TEST(*l.begin() == 100);

  l.popFront();
  BOOST_TEST(l.empty());
}

BOOST_AUTO_TEST_CASE(test_pop_back_behavior)
{
  List< int > l;
  l.pushFront(3);
  l.pushFront(2);
  l.pushFront(1);

  l.popBack();

  List< int >::iter it = l.begin();
  BOOST_TEST(*it == 1);
  ++it;
  BOOST_TEST(*it == 2);
  ++it;
  BOOST_TEST(it == l.end());
}

BOOST_AUTO_TEST_CASE(test_print_seq_get_max_size)
{
  List< namedSequence > seqs;

  namedSequence s1;
  s1.name = "seq1";
  s1.numbers.pushFront(1);

  namedSequence s2;
  s2.name = "seq2";
  s2.numbers.pushFront(3);
  s2.numbers.pushFront(2);

  seqs.pushFront(s2);
  seqs.pushFront(s1);

  BOOST_TEST(getMaxSize(seqs) == 2);
}

BOOST_AUTO_TEST_CASE(test_print_seq_get_row_sum)
{
  List< namedSequence > seqs;

  namedSequence s1;
  s1.name = "s1";
  s1.numbers.pushFront(10);

  namedSequence s2;
  s2.name = "s2";
  s2.numbers.pushFront(25);

  seqs.pushFront(s2);
  seqs.pushFront(s1);

  BOOST_TEST(getRowSum(seqs, 0) == 35);
}
