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

  template< class T >
  void Stack< T >::push(const T& value)
  {
    data_.pushFront(value);
  }

  template< class T >
  T Stack< T >::drop()
  {
    if (empty()) {
      throw std::logic_error("stack is empty");
    }

    T value = *data_.begin();
    data_.popFront();
    return value;
  }

  template< class T >
  T& Stack< T >::top()
  {
    if (empty()) {
      throw std::logic_error("stack is empty");
    }
    return *data_.begin();
  }

  template< class T >
  const T& Stack< T >::top() const
  {
    if (empty()) {
      throw std::logic_error("stack is empty");
    }
    return *data_.begin();
  }

  template< class T >
  bool Stack< T >::empty() const
  {
    return data_.empty();
  }
}

#endif
