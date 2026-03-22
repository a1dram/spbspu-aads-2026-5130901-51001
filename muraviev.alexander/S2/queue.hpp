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
}

#endif
