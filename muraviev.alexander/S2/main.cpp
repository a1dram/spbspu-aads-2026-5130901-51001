#include <iostream>
#include <string>
#include "expression.hpp"
#include "stack.hpp"

int main()
{
  muraviev::Stack< long long > results;

  std::string line;
  while (std::getline(std::cin, line)) {
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

  return 0;
}
