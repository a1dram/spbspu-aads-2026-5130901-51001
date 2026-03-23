#ifndef SEQUENCE_HPP
#define SEQUENCE_HPP

#include <stddef.h>

#include <string>
#include <utility>

#include "list.hpp"

namespace muraviev
{
  struct namedSequence
  {
    std::string name;
    List< size_t > numbers;
  };
}

#endif
