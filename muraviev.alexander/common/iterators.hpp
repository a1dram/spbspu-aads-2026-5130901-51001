#ifndef ITERATORS_HPP
#define ITERATORS_HPP

#include "node.hpp"

namespace muraviev
{
  template< class T >
  class List;

  template< class T >
  class LCIter;

  template< class T >
  class LIter
  {
    friend class List< T >;
    friend class LCIter< T >;
  public:
    LIter();

    T& operator*() const;
    T* operator->() const;

    LIter& operator++();

    bool operator==(const LIter& other) const;
    bool operator!=(const LIter& other) const;

  private:
    explicit LIter(Node< T >* node, Node< T >* head);

    Node< T >* node_;
    Node< T >* head_;
  };

  template< class T >
  class LCIter
  {
    friend class List< T >;
  public:
    LCIter();
    LCIter(const LIter< T >& other);

    const T& operator*() const;
    const T* operator->() const;

    LCIter& operator++();

    bool operator==(const LCIter& other) const;
    bool operator!=(const LCIter& other) const;

  private:
    explicit LCIter(const Node< T >* node, const Node< T >* head);

    const Node< T >* node_;
    const Node< T >* head_;
  };

  template< class T >
  LIter< T >::LIter():
    node_(nullptr),
    head_(nullptr)
  {}

  template< class T >
  T& LIter< T >::operator*() const
  {
    return node_->data;
  }

  template< class T >
  T* LIter< T >::operator->() const
  {
    return &(node_->data);
  }

  template< class T >
  LIter< T >& LIter< T >::operator++()
  {
    if (node_ != nullptr) {
      node_ = node_->next;
      if (node_ == head_) {
        node_ = nullptr;
      }
    }
    return *this;
  }

  template< class T >
  bool LIter< T >::operator==(const LIter& other) const
  {
    return node_ == other.node_ && head_ == other.head_;
  }

  template< class T >
  bool LIter< T >::operator!=(const LIter& other) const
  {
    return !(*this == other);
  }

  template< class T >
  LIter< T >::LIter(Node< T >* node, Node< T >* head):
    node_(node),
    head_(head)
  {}

  template< class T >
  LCIter< T >::LCIter():
    node_(nullptr),
    head_(nullptr)
  {}

  template< class T >
  LCIter< T >::LCIter(const LIter< T >& other):
    node_(other.node_),
    head_(other.head_)
  {}

  template< class T >
  const T& LCIter< T >::operator*() const
  {
    return node_->data;
  }

  template< class T >
  const T* LCIter< T >::operator->() const
  {
    return &(node_->data);
  }

  template< class T >
  LCIter< T >& LCIter< T >::operator++()
  {
    if (node_ != nullptr) {
      node_ = node_->next;
      if (node_ == head_) {
        node_ = nullptr;
      }
    }
    return *this;
  }

  template< class T >
  bool LCIter< T >::operator==(const LCIter& other) const
  {
    return node_ == other.node_ && head_ == other.head_;
  }

  template< class T >
  bool LCIter< T >::operator!=(const LCIter& other) const
  {
    return !(*this == other);
  }

  template< class T >
  LCIter< T >::LCIter(const Node< T >* node, const Node< T >* head):
    node_(node),
    head_(head)
  {}
}

#endif
