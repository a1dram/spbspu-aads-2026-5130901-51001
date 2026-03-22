#ifndef QUEUE_HPP
#define QUEUE_HPP

#include <stdexcept>

#include "common/list.hpp"

namespace muraviev
{
  template< class T >
  class Queue
  {
  public:
    void push(const T& value);
    T drop();

    T& front();
    const T& front() const;

    bool empty() const;

  private:
    List< T > data_;
  };

  template< class T >
  void Queue< T >::push(const T& value)
  {
    data_.pushFront(value);
  }

  template< class T >
  T Queue< T >::drop()
  {
    if (empty()) {
      throw std::logic_error("queue is empty");
    }

    T value = *data_.last();
    data_.popBack();
    return value;
  }
}

#endif
