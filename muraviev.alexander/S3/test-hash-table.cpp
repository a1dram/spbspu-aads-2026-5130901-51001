#include <boost/test/unit_test.hpp>

#include <stdexcept>
#include <string>

#include "hash_table.hpp"

namespace
{
  struct IntHash
  {
    size_t operator()(int value) const
    {
      return static_cast< size_t >(value);
    }
  };

  struct IntEqual
  {
    bool operator()(int lhs, int rhs) const
    {
      return lhs == rhs;
    }
  };
}

using namespace muraviev;

BOOST_AUTO_TEST_CASE(test_hash_table_rejects_zero_slots)
{
  BOOST_CHECK_THROW((HashTable< int, std::string, IntHash, IntEqual >(0)),
      std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(test_hash_table_add_has_at)
{
  HashTable< int, std::string, IntHash, IntEqual > table(2);

  table.add(1, "one");
  table.add(3, "three");

  BOOST_TEST(table.size() == 2);
  BOOST_TEST(table.has(1));
  BOOST_TEST(table.has(3));
  BOOST_TEST(table.at(1) == "one");
  BOOST_TEST(table.at(3) == "three");
}

BOOST_AUTO_TEST_CASE(test_hash_table_add_replaces_value)
{
  HashTable< int, std::string, IntHash, IntEqual > table(2);

  table.add(1, "one");
  table.add(1, "uno");

  BOOST_TEST(table.size() == 1);
  BOOST_TEST(table.at(1) == "uno");
}

BOOST_AUTO_TEST_CASE(test_hash_table_drop_removes_value)
{
  HashTable< int, std::string, IntHash, IntEqual > table(2);

  table.add(1, "one");
  table.add(3, "three");

  BOOST_TEST(table.drop(3) == "three");
  BOOST_TEST(!table.has(3));
  BOOST_TEST(table.size() == 1);
}

BOOST_AUTO_TEST_CASE(test_hash_table_missing_key_throws)
{
  HashTable< int, std::string, IntHash, IntEqual > table(2);

  BOOST_CHECK_THROW(table.at(10), std::out_of_range);
  BOOST_CHECK_THROW(table.drop(10), std::out_of_range);
}

BOOST_AUTO_TEST_CASE(test_hash_table_rehash_keeps_values)
{
  HashTable< int, std::string, IntHash, IntEqual > table(2);

  table.add(1, "one");
  table.add(3, "three");
  table.rehash(7);

  BOOST_TEST(table.size() == 2);
  BOOST_TEST(table.at(1) == "one");
  BOOST_TEST(table.at(3) == "three");
}

BOOST_AUTO_TEST_CASE(test_hash_table_iterator_visits_all_entries)
{
  HashTable< int, std::string, IntHash, IntEqual > table(2);
  size_t count = 0;

  table.add(1, "one");
  table.add(2, "two");
  table.add(3, "three");

  for (HashTable< int, std::string, IntHash, IntEqual >::iterator it = table.begin();
      it != table.end(); ++it) {
    ++count;
  }

  BOOST_TEST(count == table.size());
}
