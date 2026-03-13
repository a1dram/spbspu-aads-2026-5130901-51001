#include "print_seq.hpp"

#include <limits>
#include <stdexcept>

size_t muraviev::getListSize(const List< size_t >& numbers)
{
  size_t size = 0;

  for (List< size_t >::c_iter it = numbers.begin(); it != numbers.end(); ++it) {
    ++size;
  }

  return size;
}

size_t muraviev::getValueAt(const List< size_t >& numbers, size_t index)
{
  size_t currentIndex = 0;

  for (List< size_t >::c_iter it = numbers.begin(); it != numbers.end(); ++it) {
    if (currentIndex == index) {
      return *it;
    }
    ++currentIndex;
  }

  return 0;
}

