#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>

#include "expression.hpp"
#include "stack.hpp"

int main(int argc, char* argv[])
{
  std::istream* in = &std::cin;
  std::ifstream file;

  if (argc >= 2) {
    file.open(argv[1]);
    if (!file) {
      std::cerr << "cannot open file" << '\n';
      return 1;
    }
    in = &file;
  }

  muraviev::Stack< long long > results;

  try {
    std::string line;
    while (std::getline(*in, line)) {
      if (line.empty()) {
        continue;
      }

      muraviev::TokenArray tokens = muraviev::splitTokens(line);
      if (tokens.size == 0) {
        continue;
      }

      muraviev::Queue< std::string > postfix = muraviev::infixToPostfix(tokens);
      long long value = muraviev::evaluatePostfix(postfix);
      results.push(value);
    }

    bool first = true;
    while (!results.empty()) {
      if (!first) {
        std::cout << ' ';
      }
      first = false;
      std::cout << results.drop();
    }

    if (!first) {
      std::cout << '\n';
    }
  } catch (const std::exception& e) {
    std::cerr << e.what() << '\n';
    return 2;
  }

  return 0;
}
