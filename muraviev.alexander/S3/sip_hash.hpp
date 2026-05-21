#ifndef SIP_HASH_HPP
#define SIP_HASH_HPP

#include <cstddef>
#include <cstdint>

namespace muraviev
{
  std::uint64_t sipHash24(const unsigned char* data, size_t size);
}

#endif
