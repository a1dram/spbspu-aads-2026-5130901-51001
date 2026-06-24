#ifndef PRINT_SEQ_HPP
#define PRINT_SEQ_HPP

#include <ostream>

#include "sequence.hpp"

namespace muraviev
{
  size_t getListSize(const List< size_t >& numbers);
  size_t getMaxSize(const List< namedSequence >& sequences);
  bool hasIndex(const List< size_t >& numbers, size_t index);
  size_t getValueAt(const List< size_t >& numbers, size_t index);
  bool checkRowSum(const List< namedSequence >& sequences, size_t index);
  size_t getRowSum(const List< namedSequence >& sequences, size_t index);

  void printNames(std::ostream& out, const List< namedSequence >& sequences);
  void printRows(std::ostream& out, const List< namedSequence >& sequences);
  void printSums(std::ostream& out, const List< namedSequence >& sequences);
}

#endif
