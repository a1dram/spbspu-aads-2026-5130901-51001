#ifndef COMMANDS_HPP
#define COMMANDS_HPP

#include <iosfwd>

#include "storage.hpp"

namespace muraviev
{
  void executeCommands(std::istream& input, std::ostream& output,
      CommandContext& context);
}

#endif
