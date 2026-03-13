#include <iostream>
#include <stdexcept>

#include "print_seq.hpp"
#include "read_seq.hpp"

int main()
{
  muraviev::List< muraviev::namedSequence > sequences = muraviev::readSequences(std::cin);
  if (sequences.empty()) {
    std::cout << 0 << '\n';
    return 0;
  }

  try {
    muraviev::printNames(std::cout, sequences);
    muraviev::printRows(std::cout, sequences);
    muraviev::printSums(std::cout, sequences);
  } catch (const std::overflow_error& e) {
    std::cerr << e.what() << '\n';
    return 1;
  }

  return 0;
}
