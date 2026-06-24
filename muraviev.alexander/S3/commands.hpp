#ifndef COMMANDS_HPP
#define COMMANDS_HPP

#include <iosfwd>
#include <string>

#include "graph.hpp"

namespace muraviev
{
  using GraphTable = HashTable< std::string, Graph, StringHash, StringEqual >;

  bool loadGraphs(const std::string& filename, GraphTable& graphs);
  void executeCommands(std::istream& input, std::ostream& output, GraphTable& graphs);
}

#endif
