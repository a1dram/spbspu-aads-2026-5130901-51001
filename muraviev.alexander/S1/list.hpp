#ifndef LIST_HPP
#define LIST_HPP

#include <functional>

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

    void splice(iter pos, List& other);
    void splice(iter pos, List& other, iter it);
    void splice(iter pos, List& other, iter first, iter last);

    void sort();
    template< class Compare >
    void sort(Compare compare);

  private:
    bool contains(Node< T >* node) const;
    Node< T >* findPrevious(Node< T >* node) const;
    void insertNodes(Node< T >* pos, Node< T >* first, Node< T >* last);
    void detachNodes(Node< T >* first, Node< T >* last);

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

  template< class T >
  typename List< T >::iter List< T >::begin()
  {
    return iter(head_, head_);
  }

  template< class T >
  typename List< T >::c_iter List< T >::begin() const
  {
    return c_iter(head_, head_);
  }

  template< class T >
  typename List< T >::iter List< T >::end()
  {
    return iter(nullptr, head_);
  }

  template< class T >
  typename List< T >::c_iter List< T >::end() const
  {
    return c_iter(nullptr, head_);
  }

  template< class T >
  typename List< T >::iter List< T >::last()
  {
    return iter(tail_, head_);
  }

  template< class T >
  typename List< T >::c_iter List< T >::last() const
  {
    return c_iter(tail_, head_);
  }

  template< class T >
  bool List< T >::empty() const
  {
    return head_ == nullptr;
  }

  template< class T >
  void List< T >::pushFront(const T& value)
  {
    Node< T >* newNode = new Node< T >(value, head_);
    head_ = newNode;
    if (tail_ == nullptr) {
      tail_ = newNode;
      tail_->next = head_;
    } else {
      tail_->next = head_;
    }
  }

  template< class T >
  void List< T >::insert(iter pos, const T& value)
  {
    if (empty()) {
      pushFront(value);
      return;
    }
    if (pos.node_ == nullptr) {
      return;
    }

    Node< T >* newNode = new Node< T >(value, pos.node_->next);
    pos.node_->next = newNode;
    if (tail_ == pos.node_) {
      tail_ = newNode;
    }
    if (tail_ != nullptr) {
      tail_->next = head_;
    }
  }

  template< class T >
  void List< T >::popFront()
  {
    if (empty()) {
      return;
    }

    if (head_ == tail_) {
      delete head_;
      head_ = nullptr;
      tail_ = nullptr;
      return;
    }

    Node< T >* oldHead = head_;
    head_ = head_->next;
    delete oldHead;
    if (tail_ != nullptr) {
      tail_->next = head_;
    }
  }

  template< class T >
  void List< T >::popBack()
  {
    if (empty()) {
      return;
    }
    if (head_ == tail_) {
      delete head_;
      head_ = nullptr;
      tail_ = nullptr;
      return;
    }

    Node< T >* prev = head_;
    while (prev->next != tail_) {
      prev = prev->next;
    }

    delete tail_;
    tail_ = prev;
    tail_->next = head_;
  }

  template< class T >
  typename List< T >::iter List< T >::erase(iter pos)
  {
    if (empty() || pos.node_ == nullptr) {
      return end();
    }
    if (pos.node_ == head_) {
      popFront();
      return begin();
    }

    Node< T >* prev = head_;
    while (prev != nullptr && prev->next != pos.node_) {
      prev = prev->next;
    }
    if (prev == nullptr) {
      return end();
    }

    Node< T >* nextNode = pos.node_->next;
    prev->next = nextNode;
    if (tail_ == pos.node_) {
      tail_ = prev;
    }
    delete pos.node_;
    
    if (head_ == nullptr) {
      tail_ = nullptr;
      return end();
    }

    if (tail_ != nullptr) {
      tail_->next = head_;
    }

    return iter(nextNode, head_);
  }

  template< class T >
  void List< T >::clear()
  {
    if (empty()) {
      return;
    }

    tail_->next = nullptr;
    while (head_ != nullptr) {
      Node< T >* tmp = head_->next;
      delete head_;
      head_ = tmp;
    }
    head_ = nullptr;
    tail_ = nullptr;
  }

  template< class T >
  void List< T >::splice(iter pos, List& other)
  {
    if (other.empty() || this == &other) {
      return;
    }
    if (pos.node_ != nullptr && !contains(pos.node_)) {
      return;
    }

    Node< T >* first = other.head_;
    Node< T >* last = other.tail_;
    other.head_ = nullptr;
    other.tail_ = nullptr;
    insertNodes(pos.node_, first, last);
  }

  template< class T >
  void List< T >::splice(iter pos, List& other, iter it)
  {
    iter last = it;
    ++last;
    splice(pos, other, it, last);
  }

  template< class T >
  void List< T >::splice(iter pos, List& other, iter first, iter last)
  {
    if (first == last || first.node_ == nullptr || other.empty()) {
      return;
    }
    if (pos.node_ != nullptr && !contains(pos.node_)) {
      return;
    }
    if (!other.contains(first.node_)) {
      return;
    }

    Node< T >* lastNode = first.node_;
    while (lastNode != other.tail_ && lastNode->next != last.node_) {
      lastNode = lastNode->next;
    }
    if (lastNode->next != last.node_ && last.node_ != nullptr) {
      return;
    }

    if (this == &other) {
      Node< T >* current = first.node_;
      while (true) {
        if (current == pos.node_) {
          return;
        }
        if (current == lastNode) {
          break;
        }
        current = current->next;
      }

      Node< T >* previous = findPrevious(first.node_);
      if (pos.node_ == previous || (pos.node_ == nullptr && first.node_ == head_)) {
        return;
      }
    }

    Node< T >* firstNode = first.node_;
    other.detachNodes(firstNode, lastNode);
    insertNodes(pos.node_, firstNode, lastNode);
  }

  template< class T >
  void List< T >::sort()
  {
    sort(std::less< T >());
  }

  template< class T >
  template< class Compare >
  void List< T >::sort(Compare compare)
  {
    if (empty() || head_ == tail_) {
      return;
    }

    tail_->next = nullptr;
    Node< T >* sorted = nullptr;
    Node< T >* current = head_;

    try {
      while (current != nullptr) {
        Node< T >* next = current->next;
        if (sorted == nullptr || compare(current->data, sorted->data)) {
          current->next = sorted;
          sorted = current;
        } else {
          Node< T >* place = sorted;
          while (place->next != nullptr && !compare(current->data, place->next->data)) {
            place = place->next;
          }
          current->next = place->next;
          place->next = current;
        }
        current = next;
      }
    } catch (...) {
      if (sorted == nullptr) {
        sorted = current;
      } else {
        Node< T >* sortedLast = sorted;
        while (sortedLast->next != nullptr) {
          sortedLast = sortedLast->next;
        }
        sortedLast->next = current;
      }

      head_ = sorted;
      tail_ = head_;
      while (tail_->next != nullptr) {
        tail_ = tail_->next;
      }
      tail_->next = head_;
      throw;
    }

    head_ = sorted;
    tail_ = head_;
    while (tail_->next != nullptr) {
      tail_ = tail_->next;
    }
    tail_->next = head_;
  }

  template< class T >
  bool List< T >::contains(Node< T >* node) const
  {
    if (node == nullptr || empty()) {
      return false;
    }

    Node< T >* current = head_;
    while (true) {
      if (current == node) {
        return true;
      }
      if (current == tail_) {
        break;
      }
      current = current->next;
    }
    return false;
  }

  template< class T >
  Node< T >* List< T >::findPrevious(Node< T >* node) const
  {
    if (node == nullptr || empty()) {
      return nullptr;
    }

    Node< T >* current = head_;
    while (current != tail_ && current->next != node) {
      current = current->next;
    }
    if (current->next == node) {
      return current;
    }
    return nullptr;
  }

  template< class T >
  void List< T >::insertNodes(Node< T >* pos, Node< T >* first, Node< T >* last)
  {
    if (empty()) {
      head_ = first;
      tail_ = last;
      tail_->next = head_;
      return;
    }

    if (pos == nullptr) {
      last->next = head_;
      head_ = first;
      tail_->next = head_;
      return;
    }

    last->next = pos->next;
    pos->next = first;
    if (pos == tail_) {
      tail_ = last;
    }
    tail_->next = head_;
  }

  template< class T >
  void List< T >::detachNodes(Node< T >* first, Node< T >* last)
  {
    Node< T >* previous = findPrevious(first);
    Node< T >* next = last == tail_ ? head_ : last->next;

    if (first == head_ && last == tail_) {
      head_ = nullptr;
      tail_ = nullptr;
    } else {
      if (first == head_) {
        head_ = next;
      } else {
        previous->next = next;
      }
      if (last == tail_) {
        tail_ = previous;
      }
      tail_->next = head_;
    }
    last->next = nullptr;
  }
}

#endif
