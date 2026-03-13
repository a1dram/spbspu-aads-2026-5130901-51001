#include "read_seq.hpp"

#include <cstdio>
#include <string>

bool muraviev::isLineSpace(int ch)
{
  return ch == ' ' || ch == '\t' || ch == '\r';
}

bool muraviev::isLineFinished(std::istream& in)
{
  int ch = in.peek();
  return !in || ch == '\n' || ch == EOF;
}

void muraviev::skipLineSpaces(std::istream& in)
{
  while (in && isLineSpace(in.peek())) {
    in.get();
  }
}

void muraviev::pushNumber(List< size_t >& numbers, size_t value)
{
  if (numbers.empty()) {
    numbers.pushFront(value);
  } else {
    numbers.insert(numbers.last(), value);
  }
}

void muraviev::pushSequence(List< namedSequence >& sequences, const namedSequence& sequence)
{
  if (sequences.empty()) {
    sequences.pushFront(sequence);
  } else {
    sequences.insert(sequences.last(), sequence);
  }
}

muraviev::List< muraviev::namedSequence > muraviev::readSequences(std::istream& in)
{
  List< namedSequence > sequences = {};

  while (in) {
    std::string name = "";
    if (!(in >> name)) {
      break;
    }

    namedSequence seq = {};
    seq.name = name;

    while (in) {
      skipLineSpaces(in);
      if (isLineFinished(in)) {
        break;
      }

      size_t value = 0;
      in >> value;
      if (!in) {
        break;
      }
      pushNumber(seq.numbers, value);
    }

    skipLineSpaces(in);
    if (in.peek() == '\n') {
      in.get();
    }

    pushSequence(sequences, seq);
  }

  return sequences;
}
