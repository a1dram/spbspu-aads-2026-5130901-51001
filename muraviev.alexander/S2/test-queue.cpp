#include <boost/test/unit_test.hpp>

#include "queue.hpp"

using namespace muraviev;

BOOST_AUTO_TEST_CASE(test_queue_empty_after_default_constructor)
{
  Queue< int > q;
  BOOST_TEST(q.empty());
}

BOOST_AUTO_TEST_CASE(test_queue_push_front_drop)
{
  Queue< int > q;
  q.push(1);
  q.push(2);

  BOOST_TEST(q.front() == 1);
  BOOST_TEST(q.drop() == 1);
  BOOST_TEST(q.front() == 2);
  BOOST_TEST(q.drop() == 2);
  BOOST_TEST(q.empty());
}

BOOST_AUTO_TEST_CASE(test_queue_fifo_order)
{
  Queue< int > q;
  q.push(1);
  q.push(2);
  q.push(3);

  BOOST_TEST(q.drop() == 1);
  BOOST_TEST(q.drop() == 2);
  BOOST_TEST(q.drop() == 3);
}

BOOST_AUTO_TEST_CASE(test_queue_drop_from_empty_throws)
{
  Queue< int > q;
  BOOST_CHECK_THROW(q.drop(), std::logic_error);
}

BOOST_AUTO_TEST_CASE(test_queue_front_on_empty_throws)
{
  Queue< int > q;
  BOOST_CHECK_THROW(q.front(), std::logic_error);
}
