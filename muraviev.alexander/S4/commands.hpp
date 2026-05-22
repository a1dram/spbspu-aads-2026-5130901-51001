#ifndef COMMANDS_HPP
#define COMMANDS_HPP

#include <string>

#include "dataset.hpp"

namespace muraviev
{
  bool loadDatasets(const std::string& filename, DatasetTable& datasets);
}

#endif
