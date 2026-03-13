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

bool muraviev::hasIndex(const List< size_t >& numbers, size_t index)
{
  size_t currentIndex = 0;

  for (List< size_t >::c_iter it = numbers.begin(); it != numbers.end(); ++it) {
    if (currentIndex == index) {
      return true;
    }
    ++currentIndex;
  }

  return false;
}

size_t muraviev::sumChecked(size_t left, size_t right)
{
  if (std::numeric_limits< size_t >::max() - left < right) {
    throw std::overflow_error("sum overflow");
  }
  return left + right;
}

size_t muraviev::getRowSum(const List< namedSequence >& sequences, size_t index)
{
  size_t sum = 0;

  for (List< namedSequence >::c_iter it = sequences.begin(); it != sequences.end(); ++it) {
    if (hasIndex(it->numbers, index)) {
      size_t value = getValueAt(it->numbers, index);
      sum = sumChecked(sum, value);
    }
  }

  return sum;
}
