#include <boost/test/unit_test.hpp>

#include "sip_hash.hpp"

BOOST_AUTO_TEST_CASE(test_sip_hash_known_vectors)
{
  unsigned char data[4] = {0, 1, 2, 3};

  BOOST_TEST(muraviev::sipHash24(data, 0) == 0x726fdb47dd0e0e31ULL);
  BOOST_TEST(muraviev::sipHash24(data, 1) == 0x74f839c593dc67fdULL);
  BOOST_TEST(muraviev::sipHash24(data, 2) == 0x0d6c8009d9a94f5aULL);
  BOOST_TEST(muraviev::sipHash24(data, 3) == 0x85676696d7fb7e2dULL);
  BOOST_TEST(muraviev::sipHash24(data, 4) == 0xcf2794e0277187b7ULL);
}
