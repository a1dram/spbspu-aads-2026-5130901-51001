#include <fstream>
#include <iostream>

int main(int argc, char* argv[])
{
  if (argc != 2) {
    std::cerr << "filename is required\n";
    return 1;
  }

  std::ifstream input(argv[1]);
  if (!input) {
    std::cerr << "invalid input file\n";
    return 1;
  }

  return 0;
}
