#include <exception>
#include <iostream>

#include "commands.hpp"

int main()
{
  try {
    muraviev::CommandContext context;
    muraviev::executeCommands(std::cin, std::cout, context);
  } catch (const std::exception& error) {
    std::cerr << error.what() << '\n';
    return 1;
  }
  return 0;
}
