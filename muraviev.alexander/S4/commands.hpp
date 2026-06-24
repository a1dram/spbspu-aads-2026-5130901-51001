#ifndef COMMANDS_HPP
#define COMMANDS_HPP

#include <iosfwd>
#include <string>

#include "dataset.hpp"

namespace muraviev
{
  bool loadDatasets(const std::string& filename, DatasetTable& datasets);
  void executeCommands(std::istream& input, std::ostream& output,
      DatasetTable& datasets);
}

#endif
