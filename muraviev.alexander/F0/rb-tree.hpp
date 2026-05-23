#ifndef RB_TREE_HPP
#define RB_TREE_HPP

#include <cstddef>
#include <ostream>
#include <stdexcept>

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

  struct RBNodeBase
  {
    RBNodeBase* parent;
    RBNodeBase* left;
    RBNodeBase* right;
    RBColor color;

    RBNodeBase();
  };

  template< class Key, class Value >
  struct RBNode: RBNodeBase
  {
    Key key;
    Value value;

    RBNode(const Key& nodeKey, const Value& nodeValue);
  };

  template< class Key, class Value >
  class RBConstIterator;

  template< class Key, class Value >
  class RBIterator
  {
    template< class K, class V, class Compare >
    friend class RBTree;
    friend class RBConstIterator< Key, Value >;
  public:
    using Node = RBNode< Key, Value >;

    RBIterator();

    Node& operator*() const;
    Node* operator->() const;
    RBIterator& operator++();
    bool operator==(const RBIterator& other) const;
    bool operator!=(const RBIterator& other) const;

  private:
    RBIterator(RBNodeBase* node, RBNodeBase* fakeRoot);

    RBNodeBase* node_;
    RBNodeBase* fakeRoot_;
  };

  template< class Key, class Value >
  class RBConstIterator
  {
    template< class K, class V, class Compare >
    friend class RBTree;
  public:
    using Node = RBNode< Key, Value >;

    RBConstIterator();
    RBConstIterator(const RBIterator< Key, Value >& other);

    const Node& operator*() const;
    const Node* operator->() const;
    RBConstIterator& operator++();
    bool operator==(const RBConstIterator& other) const;
    bool operator!=(const RBConstIterator& other) const;

  private:
    RBConstIterator(const RBNodeBase* node, const RBNodeBase* fakeRoot);

    const RBNodeBase* node_;
    const RBNodeBase* fakeRoot_;
  };

  template< class Key, class Value >
  std::ostream& operator<<(std::ostream& output, const RBIterator< Key, Value >&)
  {
    return output << "<RBIterator>";
  }

  template< class Key, class Value >
  std::ostream& operator<<(std::ostream& output,
      const RBConstIterator< Key, Value >&)
  {
    return output << "<RBConstIterator>";
  }

  template< class Key, class Value, class Compare >
  class RBTree
  {
  public:
    using iterator = RBIterator< Key, Value >;
    using const_iterator = RBConstIterator< Key, Value >;
    using Node = RBNode< Key, Value >;

    RBTree();
    RBTree(const RBTree& other);
    RBTree(RBTree&& other);
    RBTree& operator=(const RBTree& other);
    RBTree& operator=(RBTree&& other);
    ~RBTree();

    void push(const Key& key, const Value& value);
    Value& get(const Key& key);
    const Value& get(const Key& key) const;
    Value drop(const Key& key);
    bool contains(const Key& key) const;
    bool empty() const;
    size_t size() const;
    void clear();

    iterator begin();
    iterator end();
    const_iterator begin() const;
    const_iterator end() const;
    const_iterator cbegin() const;
    const_iterator cend() const;

    bool valid() const;

  private:
    RBNodeBase* fakeRoot_;
    size_t size_;
    Compare compare_;

    RBNodeBase* root() const;
    RBNodeBase* findNode(const Key& key) const;
    RBNodeBase* getMin(RBNodeBase* node) const;
    const RBNodeBase* getMin(const RBNodeBase* node) const;
    void setRoot(RBNodeBase* node);
    void rotateLeft(RBNodeBase* node);
    void rotateRight(RBNodeBase* node);
    void fixInsert(RBNodeBase* node);
    void transplant(RBNodeBase* oldNode, RBNodeBase* newNode);
    void fixDrop(RBNodeBase* node, RBNodeBase* parent);
    void deleteSubtree(RBNodeBase* node);
    RBNodeBase* cloneSubtree(const RBNodeBase* node, RBNodeBase* parent);
    void swap(RBTree& other);
    int validateSubtree(const RBNodeBase* node, const Key* minKey,
        const Key* maxKey) const;
    static RBColor colorOf(const RBNodeBase* node);
  };

  inline RBNodeBase::RBNodeBase():
    parent(nullptr),
    left(nullptr),
    right(nullptr),
    color(BLACK)
  {}

  template< class Key, class Value >
  RBNode< Key, Value >::RBNode(const Key& nodeKey, const Value& nodeValue):
    RBNodeBase(),
    key(nodeKey),
    value(nodeValue)
  {
    color = RED;
  }

  template< class Key, class Value >
  RBIterator< Key, Value >::RBIterator():
    node_(nullptr),
    fakeRoot_(nullptr)
  {}

  template< class Key, class Value >
  typename RBIterator< Key, Value >::Node&
  RBIterator< Key, Value >::operator*() const
  {
    return *static_cast< Node* >(node_);
  }

  template< class Key, class Value >
  typename RBIterator< Key, Value >::Node*
  RBIterator< Key, Value >::operator->() const
  {
    return static_cast< Node* >(node_);
  }

  template< class Key, class Value >
  RBIterator< Key, Value >& RBIterator< Key, Value >::operator++()
  {
    if (node_ == nullptr || node_ == fakeRoot_) {
      return *this;
    }
    if (node_->right != nullptr) {
      node_ = node_->right;
      while (node_->left != nullptr) {
        node_ = node_->left;
      }
      return *this;
    }
    RBNodeBase* parent = node_->parent;
    while (parent != nullptr && parent != fakeRoot_ && node_ == parent->right) {
      node_ = parent;
      parent = parent->parent;
    }
    node_ = (parent == nullptr) ? fakeRoot_ : parent;
    return *this;
  }

  template< class Key, class Value >
  bool RBIterator< Key, Value >::operator==(const RBIterator& other) const
  {
    return node_ == other.node_ && fakeRoot_ == other.fakeRoot_;
  }

  template< class Key, class Value >
  bool RBIterator< Key, Value >::operator!=(const RBIterator& other) const
  {
    return !(*this == other);
  }

  template< class Key, class Value >
  RBIterator< Key, Value >::RBIterator(RBNodeBase* node, RBNodeBase* fakeRoot):
    node_(node),
    fakeRoot_(fakeRoot)
  {}

  template< class Key, class Value >
  RBConstIterator< Key, Value >::RBConstIterator():
    node_(nullptr),
    fakeRoot_(nullptr)
  {}

  template< class Key, class Value >
  RBConstIterator< Key, Value >::RBConstIterator(
      const RBIterator< Key, Value >& other):
    node_(other.node_),
    fakeRoot_(other.fakeRoot_)
  {}

  template< class Key, class Value >
  const typename RBConstIterator< Key, Value >::Node&
  RBConstIterator< Key, Value >::operator*() const
  {
    return *static_cast< const Node* >(node_);
  }

  template< class Key, class Value >
  const typename RBConstIterator< Key, Value >::Node*
  RBConstIterator< Key, Value >::operator->() const
  {
    return static_cast< const Node* >(node_);
  }

  template< class Key, class Value >
  RBConstIterator< Key, Value >& RBConstIterator< Key, Value >::operator++()
  {
    if (node_ == nullptr || node_ == fakeRoot_) {
      return *this;
    }
    if (node_->right != nullptr) {
      node_ = node_->right;
      while (node_->left != nullptr) {
        node_ = node_->left;
      }
      return *this;
    }
    const RBNodeBase* parent = node_->parent;
    while (parent != nullptr && parent != fakeRoot_ && node_ == parent->right) {
      node_ = parent;
      parent = parent->parent;
    }
    node_ = (parent == nullptr) ? fakeRoot_ : parent;
    return *this;
  }

  template< class Key, class Value >
  bool RBConstIterator< Key, Value >::operator==(
      const RBConstIterator& other) const
  {
    return node_ == other.node_ && fakeRoot_ == other.fakeRoot_;
  }

  template< class Key, class Value >
  bool RBConstIterator< Key, Value >::operator!=(
      const RBConstIterator& other) const
  {
    return !(*this == other);
  }

  template< class Key, class Value >
  RBConstIterator< Key, Value >::RBConstIterator(const RBNodeBase* node,
      const RBNodeBase* fakeRoot):
    node_(node),
    fakeRoot_(fakeRoot)
  {}

  template< class Key, class Value, class Compare >
}

#endif
