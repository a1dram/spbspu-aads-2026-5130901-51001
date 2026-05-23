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

    RBNodeBase(): parent(0), left(0), right(0), color(BLACK) {}
  };

  template< class Key, class Value >
  struct RBNode: RBNodeBase
  {
    Key key;
    Value value;

    RBNode(const Key& nodeKey, const Value& nodeValue):
      RBNodeBase(),
      key(nodeKey),
      value(nodeValue)
    {
      color = RED;
    }
  };

  template< class Key, class Value, class Compare >
  class RBTree
  {
  public:
    using Node = RBNode< Key, Value >;
    class iterator
    {
    public:
      iterator(): node_(0), end_(0) {}
      Node& operator*() const { return *static_cast< Node* >(node_); }
      Node* operator->() const { return static_cast< Node* >(node_); }
      iterator& operator++()
      {
        if (node_ == 0 || node_ == end_) { return *this; }
        if (node_->right != 0) {
          node_ = node_->right;
          while (node_->left != 0) { node_ = node_->left; }
          return *this;
        }
        RBNodeBase* parent = node_->parent;
        while (parent != 0 && parent != end_ && node_ == parent->right) {
          node_ = parent;
          parent = parent->parent;
        }
        node_ = parent == 0 ? end_ : parent;
        return *this;
      }
      bool operator==(const iterator& other) const { return node_ == other.node_ && end_ == other.end_; }
      bool operator!=(const iterator& other) const { return !(*this == other); }
    private:
      friend class RBTree;
      iterator(RBNodeBase* node, RBNodeBase* end): node_(node), end_(end) {}
      RBNodeBase* node_;
      RBNodeBase* end_;
    };
    class const_iterator
    {
    public:
      const_iterator(): node_(0), end_(0) {}
      const_iterator(const iterator& other): node_(other.node_), end_(other.end_) {}
      const Node& operator*() const { return *static_cast< const Node* >(node_); }
      const Node* operator->() const { return static_cast< const Node* >(node_); }
      const_iterator& operator++()
      {
        if (node_ == 0 || node_ == end_) { return *this; }
        if (node_->right != 0) {
          node_ = node_->right;
          while (node_->left != 0) { node_ = node_->left; }
          return *this;
        }
        const RBNodeBase* parent = node_->parent;
        while (parent != 0 && parent != end_ && node_ == parent->right) {
          node_ = parent;
          parent = parent->parent;
        }
        node_ = parent == 0 ? end_ : parent;
        return *this;
      }
      bool operator==(const const_iterator& other) const { return node_ == other.node_ && end_ == other.end_; }
      bool operator!=(const const_iterator& other) const { return !(*this == other); }
    private:
      friend class RBTree;
      const_iterator(const RBNodeBase* node, const RBNodeBase* end): node_(node), end_(end) {}
      const RBNodeBase* node_;
      const RBNodeBase* end_;
    };
    RBTree(): fakeRoot_(new RBNodeBase), size_(0), compare_() {}
    ~RBTree() { clear(); delete fakeRoot_; }
    void push(const Key& key, const Value& value)
    {
      RBNodeBase* parent = fakeRoot_;
      RBNodeBase* current = root();
      while (current != 0) {
        parent = current;
        Node* typed = static_cast< Node* >(current);
        if (compare_(key, typed->key)) { current = current->left; }
        else if (compare_(typed->key, key)) { current = current->right; }
        else { typed->value = value; return; }
      }
      Node* node = new Node(key, value);
      node->parent = parent;
      if (parent == fakeRoot_) { fakeRoot_->left = node; }
      else if (compare_(key, static_cast< Node* >(parent)->key)) { parent->left = node; }
      else { parent->right = node; }
      ++size_;
      root()->color = BLACK;
    }
    Value& get(const Key& key)
    {
      RBNodeBase* node = findNode(key);
      if (node == 0) { throw std::out_of_range("key not found"); }
      return static_cast< Node* >(node)->value;
    }
    const Value& get(const Key& key) const
    {
      const RBNodeBase* node = findNode(key);
      if (node == 0) { throw std::out_of_range("key not found"); }
      return static_cast< const Node* >(node)->value;
    }
    bool contains(const Key& key) const { return findNode(key) != 0; }
    bool empty() const { return size_ == 0; }
    size_t size() const { return size_; }
    void clear()
    {
      deleteSubtree(root());
      fakeRoot_->left = 0;
      fakeRoot_->right = 0;
      size_ = 0;
    }
    iterator begin() { RBNodeBase* first = getMin(root()); return iterator(first == 0 ? endNode() : first, endNode()); }
    iterator end() { return iterator(endNode(), endNode()); }
    const_iterator begin() const { return cbegin(); }
    const_iterator end() const { return cend(); }
    const_iterator cbegin() const { const RBNodeBase* first = getMin(root()); return const_iterator(first == 0 ? endNode() : first, endNode()); }
    const_iterator cend() const { return const_iterator(endNode(), endNode()); }
    bool valid() const
    {
      return root() == 0 || root()->color == BLACK;
    }
  private:
    RBNodeBase* fakeRoot_;
    size_t size_;
    Compare compare_;
    RBNodeBase* root() const { return fakeRoot_->left; }
    RBNodeBase* endNode() const { return fakeRoot_; }
    RBNodeBase* findNode(const Key& key) const
    {
      RBNodeBase* current = root();
      while (current != 0) {
        Node* typed = static_cast< Node* >(current);
        if (compare_(key, typed->key)) { current = current->left; }
        else if (compare_(typed->key, key)) { current = current->right; }
        else { return current; }
      }
      return 0;
    }
    RBNodeBase* getMin(RBNodeBase* node) const
    {
      if (node == 0) { return 0; }
      while (node->left != 0) { node = node->left; }
      return node;
    }
    const RBNodeBase* getMin(const RBNodeBase* node) const
    {
      if (node == 0) { return 0; }
      while (node->left != 0) { node = node->left; }
      return node;
    }
    void deleteSubtree(RBNodeBase* node)
    {
      if (node == 0) { return; }
      deleteSubtree(node->left);
      deleteSubtree(node->right);
      delete static_cast< Node* >(node);
    }
  };

  template< class Key, class Value >
  std::ostream& operator<<(std::ostream& output, const typename RBTree< Key, Value, Less< Key > >::iterator&)
  {
    return output << "<RBIterator>";
  }
}

#endif
