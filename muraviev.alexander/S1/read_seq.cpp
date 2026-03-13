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

    pushSequence(sequences, seq);
  }

  return sequences;
}
