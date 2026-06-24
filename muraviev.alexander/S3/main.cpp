#include <exception>
#include <iostream>

#include "commands.hpp"

int main(int argc, char* argv[])
{
  if (argc != 2) {
    std::cerr << "filename is required\n";
    return 1;
  }

  try {
    muraviev::GraphTable graphs(53);
    if (!muraviev::loadGraphs(argv[1], graphs)) {
      std::cerr << "invalid input file\n";
      return 1;
    }

    muraviev::executeCommands(std::cin, std::cout, graphs);
  } catch (const std::exception& error) {
    std::cerr << error.what() << '\n';
    return 1;
  }

  return 0;
}
