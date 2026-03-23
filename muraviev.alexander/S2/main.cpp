#include <iostream>
#include "expression.hpp"

void printQueue(muraviev::Queue<std::string> q)
{
  while (!q.empty()) {
    std::cout << q.drop() << ' ';
  }
  std::cout << '\n';
}

void testInfixToPostfix()
{
  using namespace muraviev;

  std::cout << "infixToPostfix:\n";

  TokenArray t1 = splitTokens("3 + 5 * 2");
  Queue<std::string> q1 = infixToPostfix(t1);
  std::cout << "3 + 5 * 2 -> ";
  printQueue(q1);

  TokenArray t2 = splitTokens("2 ** 3 ** 2");
  Queue<std::string> q2 = infixToPostfix(t2);
  std::cout << "2 ** 3 ** 2 -> ";
  printQueue(q2);

  TokenArray t3 = splitTokens("( 1 + 2");
  Queue<std::string> q3 = infixToPostfix(t3);
  std::cout << "( 1 + 2 -> ";
  printQueue(q3);
}

int main()
{
  testInfixToPostfix();
  return 0;
}
