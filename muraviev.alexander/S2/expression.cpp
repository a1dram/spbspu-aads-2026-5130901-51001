#include "expression.hpp"

namespace muraviev
{
  TokenArray::TokenArray():
    data(nullptr),
    size(0)
  {}

  TokenArray::TokenArray(const TokenArray& other):
    data(nullptr),
    size(0)
  {
    if (other.size == 0)
    {
      return;
    }

    data = new std::string[other.size];
    size = other.size;
    for (size_t i = 0; i < size; ++i)
    {
      data[i] = other.data[i];
    }
  }

  TokenArray& TokenArray::operator=(const TokenArray& other)
  {
    if (this == &other)
    {
      return *this;
    }

    delete[] data;
    data = nullptr;
    size = 0;

    if (other.size == 0)
    {
      return *this;
    }

    data = new std::string[other.size];
    size = other.size;
    for (size_t i = 0; i < size; ++i)
    {
      data[i] = other.data[i];
    }

    return *this;
  }

  TokenArray::~TokenArray()
  {
    delete[] data;
  }
}

#endif
