#include <iostream>
#include <string>

int main()
{
  std::string line;
  while (std::getline(std::cin, line)) {
    if (line.empty()) {
      continue;
    }
    std::cout << line << '\n';
  }
  return 0;
}
