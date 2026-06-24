#ifndef READ_SEQ_HPP
#define READ_SEQ_HPP

#include <istream>

#include "sequence.hpp"

namespace muraviev
{
  bool isLineSpace(int ch);
  bool isLineFinished(std::istream& in);
  void skipLineSpaces(std::istream& in);
  void pushNumber(List< size_t >& numbers, size_t value);
  void pushSequence(List< namedSequence >& sequences, const namedSequence& sequence);
  List< namedSequence > readSequences(std::istream& in);
}

#endif
