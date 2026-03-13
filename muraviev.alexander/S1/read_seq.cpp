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
