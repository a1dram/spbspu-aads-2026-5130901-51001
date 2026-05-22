#ifndef BSTREE_HPP
#define BSTREE_HPP

#include <cstddef>

namespace muraviev
{
  template< class Key, class Value, class Compare >
  class BSTree
  {
  public:
    class const_iterator
    {
    public:
      bool operator==(const const_iterator&) const
      {
        return true;
      }
      bool operator!=(const const_iterator&) const
      {
        return false;
      }
    };

    bool empty() const
    {
      return true;
    }
    size_t size() const
    {
      return 0;
    }
    const_iterator begin() const
    {
      return const_iterator();
    }
    const_iterator end() const
    {
      return const_iterator();
    }
    const_iterator cbegin() const
    {
      return begin();
    }
    const_iterator cend() const
    {
      return end();
    }
  };
}

#endif
