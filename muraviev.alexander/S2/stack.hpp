#ifndef STACK_HPP
#define STACK_HPP

#include <stdexcept>

#include "common/list.hpp"

namespace muraviev
{
  template< class T >
  class Stack
  {
  public:
    void push(const T& value);
    T drop();

    T& top();
    const T& top() const;

    bool empty() const;

  private:
    List< T > data_;
  };
}

#endif
