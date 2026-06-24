#ifndef DATASET_HPP
#define DATASET_HPP

#include <string>

#include "bstree.hpp"

namespace muraviev
{
  template< class T >
  struct Less
  {
    bool operator()(const T& left, const T& right) const
    {
      return left < right;
    }
  };

  using Dataset = BSTree< int, std::string, Less< int > >;
  using DatasetTable = BSTree< std::string, Dataset, Less< std::string > >;
}

#endif
