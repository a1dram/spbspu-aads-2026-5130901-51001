#ifndef LIST_HPP
#define LIST_HPP

#include "iterators.hpp"

namespace muraviev
{
  template< class T >
  class List
  {
  public:
    using iter = LIter< T >;
    using c_iter = LCIter< T >;

    List();
    ~List();
    List(const List& other);
    List& operator=(const List& other);

    iter begin();
    c_iter begin() const;

    iter end();
    c_iter end() const;

    iter last();
    c_iter last() const;

    bool empty() const;

    void pushFront(const T& value);
    void insert(iter pos, const T& value);
    void popFront();
    void popBack();
    iter erase(iter pos);
    void clear();

  private:
    Node< T >* head_;
    Node< T >* tail_;
  };

  template< class T >
  List< T >::List():
    head_(nullptr),
    tail_(nullptr)
  {}
  
  template< class T >
  List< T >::~List()
  {
    clear();
  }

  template< class T >
  List< T >::List(const List& other):
    head_(nullptr),
    tail_(nullptr)
  {
    for (c_iter it = other.begin(); it != other.end(); ++it) {
      if (empty()) {
        pushFront(*it);
      } else {
        insert(last(), *it);
      }
    }
  }

  template< class T >
  List< T >& List< T >::operator=(const List& other)
  {
    if (this == &other) {
      return *this;
    }

    clear();

    for (c_iter it = other.begin(); it != other.end(); ++it) {
      if (empty()) {
        pushFront(*it);
      } else {
        insert(last(), *it);
      }
    }

    return *this;
  }
}

#endif
