#define BOOST_TEST_MODULE S1
#include <boost/test/included/unit_test.hpp>

#include "list.hpp"
#include "sequence.hpp"
#include "read_seq.hpp"
#include "print_seq.hpp"

using namespace muraviev;

BOOST_AUTO_TEST_CASE(test_list_empty_after_default_constructor)
{
  List<int> lst;
  BOOST_TEST(lst.empty());
}
