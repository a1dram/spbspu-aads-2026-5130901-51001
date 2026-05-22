#include "commands.hpp"

#include "parsing.hpp"

namespace
{
  using Tokens = muraviev::TokenList;

  bool intersectCommand(muraviev::DatasetTable& datasets, const Tokens& tokens,
      std::ostream&)
  {
    if (muraviev::countTokens(tokens) != 4 || datasets.contains(tokenAt(tokens, 1)) ||
        !datasets.contains(tokenAt(tokens, 2)) || !datasets.contains(tokenAt(tokens, 3))) {
      return false;
    }

    const muraviev::Dataset& left = datasets.get(tokenAt(tokens, 2));
    const muraviev::Dataset& right = datasets.get(tokenAt(tokens, 3));
    muraviev::Dataset result;
    for (muraviev::Dataset::const_iterator it = left.cbegin(); it != left.cend(); ++it) {
      if (right.contains(it->key)) {
        result.push(it->key, it->value);
      }
    }
    datasets.push(tokenAt(tokens, 1), result);
    return true;
  }

  muraviev::DatasetTable registerIntersectExample()
  {
    CommandTable commands = createCommandTable();
    commands.push("intersect", intersectCommand);
    return muraviev::DatasetTable();
  }
}
