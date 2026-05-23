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
      fixInsert(node);
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
    Value drop(const Key& key)
    {
      RBNodeBase* removed = findNode(key);
      if (removed == 0) { throw std::out_of_range("key not found"); }
      if (removed->left != 0 || removed->right != 0) { throw std::out_of_range("node is not leaf"); }
      Value result = static_cast< Node* >(removed)->value;
      transplant(removed, 0);
      delete static_cast< Node* >(removed);
      --size_;
      if (root() != 0) { root()->color = BLACK; }
      return result;
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
      if (root() == 0) { return size_ == 0; }
      if (root()->color != BLACK) { return false; }
      return validateSubtree(root(), 0, 0) >= 0;
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
    void rotateLeft(RBNodeBase* node)
    {
      RBNodeBase* right = node->right;
      node->right = right->left;
      if (right->left != 0) { right->left->parent = node; }
      right->parent = node->parent;
      if (node->parent == endNode()) { fakeRoot_->left = right; }
      else if (node == node->parent->left) { node->parent->left = right; }
      else { node->parent->right = right; }
      right->left = node;
      node->parent = right;
    }
    void rotateRight(RBNodeBase* node)
    {
      RBNodeBase* left = node->left;
      node->left = left->right;
      if (left->right != 0) { left->right->parent = node; }
      left->parent = node->parent;
      if (node->parent == endNode()) { fakeRoot_->left = left; }
      else if (node == node->parent->right) { node->parent->right = left; }
      else { node->parent->left = left; }
      left->right = node;
      node->parent = left;
    }
    RBColor colorOf(const RBNodeBase* node) const { return node == 0 ? BLACK : node->color; }
    void fixInsert(RBNodeBase* node)
    {
      while (node->parent != endNode() && node->parent->color == RED) {
        RBNodeBase* parent = node->parent;
        RBNodeBase* grand = parent->parent;
        if (parent == grand->left) {
          RBNodeBase* uncle = grand->right;
          if (colorOf(uncle) == RED) { parent->color = BLACK; uncle->color = BLACK; grand->color = RED; node = grand; }
          else { if (node == parent->right) { node = parent; rotateLeft(node); parent = node->parent; grand = parent->parent; } parent->color = BLACK; grand->color = RED; rotateRight(grand); }
        } else {
          RBNodeBase* uncle = grand->left;
          if (colorOf(uncle) == RED) { parent->color = BLACK; uncle->color = BLACK; grand->color = RED; node = grand; }
          else { if (node == parent->left) { node = parent; rotateRight(node); parent = node->parent; grand = parent->parent; } parent->color = BLACK; grand->color = RED; rotateLeft(grand); }
        }
      }
      root()->color = BLACK;
    }
    void transplant(RBNodeBase* oldNode, RBNodeBase* newNode)
    {
      if (oldNode->parent == endNode()) { fakeRoot_->left = newNode; }
      else if (oldNode == oldNode->parent->left) { oldNode->parent->left = newNode; }
      else { oldNode->parent->right = newNode; }
      if (newNode != 0) { newNode->parent = oldNode->parent; }
    }
    void deleteSubtree(RBNodeBase* node)
    {
      if (node == 0) { return; }
      deleteSubtree(node->left);
      deleteSubtree(node->right);
      delete static_cast< Node* >(node);
    }
    int validateSubtree(const RBNodeBase* node, const Key* minKey, const Key* maxKey) const
    {
      if (node == 0) { return 1; }
      const Node* typed = static_cast< const Node* >(node);
      if ((minKey != 0 && !compare_(*minKey, typed->key)) || (maxKey != 0 && !compare_(typed->key, *maxKey))) { return -1; }
      if (node->left != 0 && node->left->parent != node) { return -1; }
      if (node->right != 0 && node->right->parent != node) { return -1; }
      if (node->color == RED && ((node->left != 0 && node->left->color == RED) || (node->right != 0 && node->right->color == RED))) { return -1; }
      int left = validateSubtree(node->left, minKey, &typed->key);
      int right = validateSubtree(node->right, &typed->key, maxKey);
      if (left < 0 || right < 0 || left != right) { return -1; }
      return left + (node->color == BLACK ? 1 : 0);
    }
  };

  template< class Key, class Value >
  std::ostream& operator<<(std::ostream& output, const typename RBTree< Key, Value, Less< Key > >::iterator&)
  {
    return output << "<RBIterator>";
  }
}

#endif
