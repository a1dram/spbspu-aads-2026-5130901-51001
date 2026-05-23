#ifndef RB_TREE_HPP
#define RB_TREE_HPP

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

  enum RBColor
  {
    RED,
    BLACK
  };
}

#endif
