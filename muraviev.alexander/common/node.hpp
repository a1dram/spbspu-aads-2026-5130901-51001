#ifndef NODE_HPP
#define NODE_HPP

namespace muraviev
{
  template< class T >
  struct Node
  {
    T data;
    Node< T >* next;

    Node(const T& value, Node< T >* nextNode = nullptr):
      data(value),
      next(nextNode)
    {}
  };
}

#endif
