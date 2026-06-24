#include "sip_hash.hpp"

namespace
{
  std::uint64_t rotateLeft(std::uint64_t value, int shift)
  {
    return (value << shift) | (value >> (64 - shift));
  }

  std::uint64_t read64(const unsigned char* data)
  {
    std::uint64_t result = 0;
    for (int i = 0; i < 8; ++i) {
      result |= static_cast< std::uint64_t >(data[i]) << (8 * i);
    }
    return result;
  }

  void sipRound(std::uint64_t& v0, std::uint64_t& v1, std::uint64_t& v2,
      std::uint64_t& v3)
  {
    v0 += v1;
    v1 = rotateLeft(v1, 13);
    v1 ^= v0;
    v0 = rotateLeft(v0, 32);

    v2 += v3;
    v3 = rotateLeft(v3, 16);
    v3 ^= v2;

    v0 += v3;
    v3 = rotateLeft(v3, 21);
    v3 ^= v0;

    v2 += v1;
    v1 = rotateLeft(v1, 17);
    v1 ^= v2;
    v2 = rotateLeft(v2, 32);
  }
}

std::uint64_t muraviev::sipHash24(const unsigned char* data, size_t size)
{
  const std::uint64_t k0 = 0x0706050403020100ULL;
  const std::uint64_t k1 = 0x0f0e0d0c0b0a0908ULL;

  std::uint64_t v0 = 0x736f6d6570736575ULL ^ k0;
  std::uint64_t v1 = 0x646f72616e646f6dULL ^ k1;
  std::uint64_t v2 = 0x6c7967656e657261ULL ^ k0;
  std::uint64_t v3 = 0x7465646279746573ULL ^ k1;

  const unsigned char* current = data;
  size_t left = size;
  while (left >= 8) {
    const std::uint64_t message = read64(current);
    v3 ^= message;
    sipRound(v0, v1, v2, v3);
    sipRound(v0, v1, v2, v3);
    v0 ^= message;
    current += 8;
    left -= 8;
  }

  std::uint64_t last = static_cast< std::uint64_t >(size) << 56;
  for (size_t i = 0; i < left; ++i) {
    last |= static_cast< std::uint64_t >(current[i]) << (8 * i);
  }

  v3 ^= last;
  sipRound(v0, v1, v2, v3);
  sipRound(v0, v1, v2, v3);
  v0 ^= last;

  v2 ^= 0xff;
  for (int i = 0; i < 4; ++i) {
    sipRound(v0, v1, v2, v3);
  }

  return v0 ^ v1 ^ v2 ^ v3;
}
