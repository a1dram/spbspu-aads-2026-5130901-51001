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

size_t muraviev::getMaxSize(const List< namedSequence >& sequences)
{
  size_t maxSize = 0;

  for (List< namedSequence >::c_iter it = sequences.begin(); it != sequences.end(); ++it) {
    size_t currentSize = getListSize(it->numbers);
    if (currentSize > maxSize) {
      maxSize = currentSize;
    }
  }

  return maxSize;
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

void muraviev::printNames(std::ostream& out, const List< namedSequence >& sequences)
{
  bool first = true;

  for (List< namedSequence >::c_iter it = sequences.begin(); it != sequences.end(); ++it) {
    if (!first) {
      out << ' ';
    }
    out << it->name;
    first = false;
  }

  out << '\n';
}

void muraviev::printRows(std::ostream& out, const List< namedSequence >& sequences)
{
  size_t maxSize = getMaxSize(sequences);

  for (size_t index = 0; index < maxSize; ++index) {
    bool first = true;

    for (List< namedSequence >::c_iter it = sequences.begin(); it != sequences.end(); ++it) {
      if (hasIndex(it->numbers, index)) {
        size_t value = getValueAt(it->numbers, index);
        if (!first) {
          out << ' ';
        }
        out << value;
        first = false;
      }
    }

    out << '\n';
  }
}

void muraviev::printSums(std::ostream& out, const List< namedSequence >& sequences)
{
  size_t maxSize = getMaxSize(sequences);

  if (maxSize == 0 && !sequences.empty()) {
    out << 0 << '\n';
    return;
  }

  bool first = true;

  for (size_t index = 0; index < maxSize; ++index) {
    size_t sum = getRowSum(sequences, index);
    if (!first) {
      out << ' ';
    }
    out << sum;
    first = false;
  }

  out << '\n';
}
