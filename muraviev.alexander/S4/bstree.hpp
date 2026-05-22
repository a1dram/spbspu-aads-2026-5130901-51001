#ifndef BSTREE_HPP
#define BSTREE_HPP

#include <cstddef>
#include <ostream>
#include <stdexcept>

namespace muraviev
{
  struct TreeNodeBase
  {
    TreeNodeBase* parent;
    TreeNodeBase* left;
    TreeNodeBase* right;

    TreeNodeBase();
  };

  template< class Key, class Value >
  struct TreeNode: TreeNodeBase
  {
    Key key;
    Value value;

    TreeNode(const Key& nodeKey, const Value& nodeValue);
  };

  template< class Key, class Value >
  class BSTConstIterator;

  template< class Key, class Value >
  class BSTIterator
  {
    template< class K, class V, class Compare >
    friend class BSTree;
    friend class BSTConstIterator< Key, Value >;
  public:
    using Node = TreeNode< Key, Value >;

    BSTIterator();

    Node& operator*() const;
    Node* operator->() const;

    BSTIterator& operator++();

    bool operator==(const BSTIterator& other) const;
    bool operator!=(const BSTIterator& other) const;

  private:
    BSTIterator(TreeNodeBase* node, TreeNodeBase* fakeRoot);

    TreeNodeBase* node_;
    TreeNodeBase* fakeRoot_;
  };

  template< class Key, class Value >
  class BSTConstIterator
  {
    template< class K, class V, class Compare >
    friend class BSTree;
  public:
    using Node = TreeNode< Key, Value >;

    BSTConstIterator();
    BSTConstIterator(const BSTIterator< Key, Value >& other);

    const Node& operator*() const;
    const Node* operator->() const;

    BSTConstIterator& operator++();

    bool operator==(const BSTConstIterator& other) const;
    bool operator!=(const BSTConstIterator& other) const;

  private:
    BSTConstIterator(const TreeNodeBase* node, const TreeNodeBase* fakeRoot);

    const TreeNodeBase* node_;
    const TreeNodeBase* fakeRoot_;
  };

  template< class Key, class Value >
  std::ostream& operator<<(std::ostream& output, const BSTIterator< Key, Value >&)
  {
    return output << "<BSTIterator>";
  }

  template< class Key, class Value >
  std::ostream& operator<<(std::ostream& output,
      const BSTConstIterator< Key, Value >&)
  {
    return output << "<BSTConstIterator>";
  }

  template< class Key, class Value, class Compare >
  class BSTree
  {
  public:
    using iterator = BSTIterator< Key, Value >;
    using const_iterator = BSTConstIterator< Key, Value >;
    using Node = TreeNode< Key, Value >;

    BSTree();
    BSTree(const BSTree& other);
    BSTree(BSTree&& other);
    BSTree& operator=(const BSTree& other);
    BSTree& operator=(BSTree&& other);
    ~BSTree();

    void push(const Key& key, const Value& value);
    Value& get(const Key& key);
    const Value& get(const Key& key) const;
    Value drop(const Key& key);

    bool contains(const Key& key) const;
    bool empty() const;
    size_t size() const;

    iterator begin();
    iterator end();
    const_iterator begin() const;
    const_iterator end() const;
    const_iterator cbegin() const;
    const_iterator cend() const;

    const_iterator rotateLeft(const_iterator it);
    const_iterator rotateRight(const_iterator it);
    const_iterator rotateLargeLeft(const_iterator it);
    const_iterator rotateLargeRight(const_iterator it);

    size_t height() const;
    size_t height(const_iterator it) const;

    void clear();

  private:
    TreeNodeBase* fakeRoot_;
    size_t size_;
    Compare compare_;

    TreeNodeBase* findNode(const Key& key) const;
    TreeNodeBase* getMin(TreeNodeBase* node) const;
    const TreeNodeBase* getMin(const TreeNodeBase* node) const;
    void attachParent(TreeNodeBase* oldNode, TreeNodeBase* newNode);
    void deleteSubtree(TreeNodeBase* node);
    TreeNodeBase* cloneSubtree(const TreeNodeBase* node, TreeNodeBase* parent);
    size_t subtreeHeight(const TreeNodeBase* node) const;
    void swap(BSTree& other);
  };

  inline TreeNodeBase::TreeNodeBase():
    parent(nullptr),
    left(nullptr),
    right(nullptr)
  {}

  template< class Key, class Value >
  TreeNode< Key, Value >::TreeNode(const Key& nodeKey, const Value& nodeValue):
    TreeNodeBase(),
    key(nodeKey),
    value(nodeValue)
  {}

  template< class Key, class Value >
  BSTIterator< Key, Value >::BSTIterator():
    node_(nullptr),
    fakeRoot_(nullptr)
  {}

  template< class Key, class Value >
  typename BSTIterator< Key, Value >::Node&
  BSTIterator< Key, Value >::operator*() const
  {
    return *static_cast< Node* >(node_);
  }

  template< class Key, class Value >
  typename BSTIterator< Key, Value >::Node*
  BSTIterator< Key, Value >::operator->() const
  {
    return static_cast< Node* >(node_);
  }

  template< class Key, class Value >
  BSTIterator< Key, Value >& BSTIterator< Key, Value >::operator++()
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

    TreeNodeBase* parent = node_->parent;
    while (parent != nullptr && parent != fakeRoot_ && node_ == parent->right) {
      node_ = parent;
      parent = parent->parent;
    }
    node_ = (parent == nullptr) ? fakeRoot_ : parent;
    return *this;
  }

  template< class Key, class Value >
  bool BSTIterator< Key, Value >::operator==(const BSTIterator& other) const
  {
    return node_ == other.node_ && fakeRoot_ == other.fakeRoot_;
  }

  template< class Key, class Value >
  bool BSTIterator< Key, Value >::operator!=(const BSTIterator& other) const
  {
    return !(*this == other);
  }

  template< class Key, class Value >
  BSTIterator< Key, Value >::BSTIterator(TreeNodeBase* node, TreeNodeBase* fakeRoot):
    node_(node),
    fakeRoot_(fakeRoot)
  {}

  template< class Key, class Value >
  BSTConstIterator< Key, Value >::BSTConstIterator():
    node_(nullptr),
    fakeRoot_(nullptr)
  {}

  template< class Key, class Value >
  BSTConstIterator< Key, Value >::BSTConstIterator(
      const BSTIterator< Key, Value >& other):
    node_(other.node_),
    fakeRoot_(other.fakeRoot_)
  {}

  template< class Key, class Value >
  const typename BSTConstIterator< Key, Value >::Node&
  BSTConstIterator< Key, Value >::operator*() const
  {
    return *static_cast< const Node* >(node_);
  }

  template< class Key, class Value >
  const typename BSTConstIterator< Key, Value >::Node*
  BSTConstIterator< Key, Value >::operator->() const
  {
    return static_cast< const Node* >(node_);
  }

  template< class Key, class Value >
  BSTConstIterator< Key, Value >& BSTConstIterator< Key, Value >::operator++()
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

    const TreeNodeBase* parent = node_->parent;
    while (parent != nullptr && parent != fakeRoot_ && node_ == parent->right) {
      node_ = parent;
      parent = parent->parent;
    }
    node_ = (parent == nullptr) ? fakeRoot_ : parent;
    return *this;
  }

  template< class Key, class Value >
  bool BSTConstIterator< Key, Value >::operator==(const BSTConstIterator& other) const
  {
    return node_ == other.node_ && fakeRoot_ == other.fakeRoot_;
  }

  template< class Key, class Value >
  bool BSTConstIterator< Key, Value >::operator!=(const BSTConstIterator& other) const
  {
    return !(*this == other);
  }

  template< class Key, class Value >
  BSTConstIterator< Key, Value >::BSTConstIterator(const TreeNodeBase* node,
      const TreeNodeBase* fakeRoot):
    node_(node),
    fakeRoot_(fakeRoot)
  {}

  template< class Key, class Value, class Compare >
  BSTree< Key, Value, Compare >::BSTree():
    fakeRoot_(new TreeNodeBase),
    size_(0),
    compare_()
  {}

  template< class Key, class Value, class Compare >
  BSTree< Key, Value, Compare >::BSTree(const BSTree& other):
    fakeRoot_(new TreeNodeBase),
    size_(other.size_),
    compare_(other.compare_)
  {
    try {
      fakeRoot_->left = cloneSubtree(other.fakeRoot_->left, fakeRoot_);
    } catch (...) {
      deleteSubtree(fakeRoot_->left);
      delete fakeRoot_;
      fakeRoot_ = nullptr;
      throw;
    }
  }

  template< class Key, class Value, class Compare >
  BSTree< Key, Value, Compare >::BSTree(BSTree&& other):
    fakeRoot_(other.fakeRoot_),
    size_(other.size_),
    compare_(other.compare_)
  {
    other.fakeRoot_ = new TreeNodeBase;
    other.size_ = 0;
  }

  template< class Key, class Value, class Compare >
  BSTree< Key, Value, Compare >&
  BSTree< Key, Value, Compare >::operator=(const BSTree& other)
  {
    if (this == &other) {
      return *this;
    }
    BSTree tmp(other);
    swap(tmp);
    return *this;
  }

  template< class Key, class Value, class Compare >
  BSTree< Key, Value, Compare >&
  BSTree< Key, Value, Compare >::operator=(BSTree&& other)
  {
    if (this == &other) {
      return *this;
    }
    clear();
    delete fakeRoot_;
    fakeRoot_ = other.fakeRoot_;
    size_ = other.size_;
    compare_ = other.compare_;
    other.fakeRoot_ = new TreeNodeBase;
    other.size_ = 0;
    return *this;
  }

  template< class Key, class Value, class Compare >
  BSTree< Key, Value, Compare >::~BSTree()
  {
    clear();
    delete fakeRoot_;
  }

  template< class Key, class Value, class Compare >
  void BSTree< Key, Value, Compare >::push(const Key& key, const Value& value)
  {
    TreeNodeBase* parent = fakeRoot_;
    TreeNodeBase* current = fakeRoot_->left;
    while (current != nullptr) {
      parent = current;
      Node* node = static_cast< Node* >(current);
      if (compare_(key, node->key)) {
        current = current->left;
      } else if (compare_(node->key, key)) {
        current = current->right;
      } else {
        node->value = value;
        return;
      }
    }

    Node* node = new Node(key, value);
    node->parent = parent;
    if (parent == fakeRoot_) {
      fakeRoot_->left = node;
    } else if (compare_(key, static_cast< Node* >(parent)->key)) {
      parent->left = node;
    } else {
      parent->right = node;
    }
    ++size_;
  }

  template< class Key, class Value, class Compare >
  Value& BSTree< Key, Value, Compare >::get(const Key& key)
  {
    TreeNodeBase* node = findNode(key);
    if (node == nullptr) {
      throw std::out_of_range("key not found");
    }
    return static_cast< Node* >(node)->value;
  }

  template< class Key, class Value, class Compare >
  const Value& BSTree< Key, Value, Compare >::get(const Key& key) const
  {
    const TreeNodeBase* node = findNode(key);
    if (node == nullptr) {
      throw std::out_of_range("key not found");
    }
    return static_cast< const Node* >(node)->value;
  }

  template< class Key, class Value, class Compare >
  Value BSTree< Key, Value, Compare >::drop(const Key& key)
  {
    TreeNodeBase* removed = findNode(key);
    if (removed == nullptr) {
      throw std::out_of_range("key not found");
    }

    Value result = static_cast< Node* >(removed)->value;
    if (removed->left == nullptr) {
      attachParent(removed, removed->right);
    } else if (removed->right == nullptr) {
      attachParent(removed, removed->left);
    } else {
      TreeNodeBase* successor = getMin(removed->right);
      if (successor->parent != removed) {
        attachParent(successor, successor->right);
        successor->right = removed->right;
        successor->right->parent = successor;
      }
      attachParent(removed, successor);
      successor->left = removed->left;
      successor->left->parent = successor;
    }

    delete static_cast< Node* >(removed);
    --size_;
    return result;
  }

  template< class Key, class Value, class Compare >
  bool BSTree< Key, Value, Compare >::contains(const Key& key) const
  {
    return findNode(key) != nullptr;
  }

  template< class Key, class Value, class Compare >
  bool BSTree< Key, Value, Compare >::empty() const
  {
    return size_ == 0;
  }

  template< class Key, class Value, class Compare >
  size_t BSTree< Key, Value, Compare >::size() const
  {
    return size_;
  }

  template< class Key, class Value, class Compare >
  typename BSTree< Key, Value, Compare >::iterator
  BSTree< Key, Value, Compare >::begin()
  {
    TreeNodeBase* first = getMin(fakeRoot_->left);
    return iterator(first == nullptr ? fakeRoot_ : first, fakeRoot_);
  }

  template< class Key, class Value, class Compare >
  typename BSTree< Key, Value, Compare >::iterator
  BSTree< Key, Value, Compare >::end()
  {
    return iterator(fakeRoot_, fakeRoot_);
  }

  template< class Key, class Value, class Compare >
  typename BSTree< Key, Value, Compare >::const_iterator
  BSTree< Key, Value, Compare >::begin() const
  {
    return cbegin();
  }

  template< class Key, class Value, class Compare >
  typename BSTree< Key, Value, Compare >::const_iterator
  BSTree< Key, Value, Compare >::end() const
  {
    return cend();
  }

  template< class Key, class Value, class Compare >
  typename BSTree< Key, Value, Compare >::const_iterator
  BSTree< Key, Value, Compare >::cbegin() const
  {
    const TreeNodeBase* first = getMin(fakeRoot_->left);
    return const_iterator(first == nullptr ? fakeRoot_ : first, fakeRoot_);
  }

  template< class Key, class Value, class Compare >
  typename BSTree< Key, Value, Compare >::const_iterator
  BSTree< Key, Value, Compare >::cend() const
  {
    return const_iterator(fakeRoot_, fakeRoot_);
  }

  template< class Key, class Value, class Compare >
  typename BSTree< Key, Value, Compare >::const_iterator
  BSTree< Key, Value, Compare >::rotateLeft(const_iterator it)
  {
    TreeNodeBase* node = const_cast< TreeNodeBase* >(it.node_);
    if (it.fakeRoot_ != fakeRoot_ || node == fakeRoot_ || node == nullptr ||
        node->parent == fakeRoot_ || node->parent == nullptr ||
        node->parent->right != node) {
      throw std::logic_error("invalid left rotation");
    }

    TreeNodeBase* parent = node->parent;
    TreeNodeBase* subtree = node->left;
    attachParent(parent, node);
    parent->right = subtree;
    if (subtree != nullptr) {
      subtree->parent = parent;
    }
    node->left = parent;
    parent->parent = node;
    return const_iterator(node, fakeRoot_);
  }

  template< class Key, class Value, class Compare >
  typename BSTree< Key, Value, Compare >::const_iterator
  BSTree< Key, Value, Compare >::rotateRight(const_iterator it)
  {
    TreeNodeBase* node = const_cast< TreeNodeBase* >(it.node_);
    if (it.fakeRoot_ != fakeRoot_ || node == fakeRoot_ || node == nullptr ||
        node->parent == fakeRoot_ || node->parent == nullptr ||
        node->parent->left != node) {
      throw std::logic_error("invalid right rotation");
    }

    TreeNodeBase* parent = node->parent;
    TreeNodeBase* subtree = node->right;
    attachParent(parent, node);
    parent->left = subtree;
    if (subtree != nullptr) {
      subtree->parent = parent;
    }
    node->right = parent;
    parent->parent = node;
    return const_iterator(node, fakeRoot_);
  }

  template< class Key, class Value, class Compare >
  typename BSTree< Key, Value, Compare >::const_iterator
  BSTree< Key, Value, Compare >::rotateLargeLeft(const_iterator it)
  {
    TreeNodeBase* node = const_cast< TreeNodeBase* >(it.node_);
    if (it.fakeRoot_ != fakeRoot_ || node == fakeRoot_ || node == nullptr ||
        node->parent == nullptr || node->parent == fakeRoot_ ||
        node->parent->parent == nullptr || node->parent->parent == fakeRoot_ ||
        node->parent->left != node || node->parent->parent->right != node->parent) {
      throw std::logic_error("invalid large left rotation");
    }
    rotateRight(it);
    return rotateLeft(const_iterator(node, fakeRoot_));
  }

  template< class Key, class Value, class Compare >
  typename BSTree< Key, Value, Compare >::const_iterator
  BSTree< Key, Value, Compare >::rotateLargeRight(const_iterator it)
  {
    TreeNodeBase* node = const_cast< TreeNodeBase* >(it.node_);
    if (it.fakeRoot_ != fakeRoot_ || node == fakeRoot_ || node == nullptr ||
        node->parent == nullptr || node->parent == fakeRoot_ ||
        node->parent->parent == nullptr || node->parent->parent == fakeRoot_ ||
        node->parent->right != node || node->parent->parent->left != node->parent) {
      throw std::logic_error("invalid large right rotation");
    }
    rotateLeft(it);
    return rotateRight(const_iterator(node, fakeRoot_));
  }

  template< class Key, class Value, class Compare >
  size_t BSTree< Key, Value, Compare >::height() const
  {
    return subtreeHeight(fakeRoot_->left);
  }

  template< class Key, class Value, class Compare >
  size_t BSTree< Key, Value, Compare >::height(const_iterator it) const
  {
    if (it.fakeRoot_ != fakeRoot_ || it.node_ == fakeRoot_ || it.node_ == nullptr) {
      return 0;
    }
    return subtreeHeight(it.node_);
  }

  template< class Key, class Value, class Compare >
  void BSTree< Key, Value, Compare >::clear()
  {
    deleteSubtree(fakeRoot_->left);
    fakeRoot_->left = nullptr;
    fakeRoot_->right = nullptr;
    fakeRoot_->parent = nullptr;
    size_ = 0;
  }

  template< class Key, class Value, class Compare >
  TreeNodeBase* BSTree< Key, Value, Compare >::findNode(const Key& key) const
  {
    TreeNodeBase* current = fakeRoot_->left;
    while (current != nullptr) {
      Node* node = static_cast< Node* >(current);
      if (compare_(key, node->key)) {
        current = current->left;
      } else if (compare_(node->key, key)) {
        current = current->right;
      } else {
        return current;
      }
    }
    return nullptr;
  }

  template< class Key, class Value, class Compare >
  TreeNodeBase* BSTree< Key, Value, Compare >::getMin(TreeNodeBase* node) const
  {
    if (node == nullptr) {
      return nullptr;
    }
    while (node->left != nullptr) {
      node = node->left;
    }
    return node;
  }

  template< class Key, class Value, class Compare >
  const TreeNodeBase* BSTree< Key, Value, Compare >::getMin(
      const TreeNodeBase* node) const
  {
    if (node == nullptr) {
      return nullptr;
    }
    while (node->left != nullptr) {
      node = node->left;
    }
    return node;
  }

  template< class Key, class Value, class Compare >
  void BSTree< Key, Value, Compare >::attachParent(TreeNodeBase* oldNode,
      TreeNodeBase* newNode)
  {
    TreeNodeBase* parent = oldNode->parent;
    if (parent == fakeRoot_) {
      fakeRoot_->left = newNode;
    } else if (parent->left == oldNode) {
      parent->left = newNode;
    } else {
      parent->right = newNode;
    }
    if (newNode != nullptr) {
      newNode->parent = parent;
    }
  }

  template< class Key, class Value, class Compare >
  void BSTree< Key, Value, Compare >::deleteSubtree(TreeNodeBase* node)
  {
    if (node == nullptr) {
      return;
    }
    deleteSubtree(node->left);
    deleteSubtree(node->right);
    delete static_cast< Node* >(node);
  }

  template< class Key, class Value, class Compare >
  TreeNodeBase* BSTree< Key, Value, Compare >::cloneSubtree(
      const TreeNodeBase* node, TreeNodeBase* parent)
  {
    if (node == nullptr) {
      return nullptr;
    }
    const Node* source = static_cast< const Node* >(node);
    Node* copy = new Node(source->key, source->value);
    copy->parent = parent;
    try {
      copy->left = cloneSubtree(node->left, copy);
      copy->right = cloneSubtree(node->right, copy);
    } catch (...) {
      deleteSubtree(copy);
      throw;
    }
    return copy;
  }

  template< class Key, class Value, class Compare >
  size_t BSTree< Key, Value, Compare >::subtreeHeight(const TreeNodeBase* node) const
  {
    if (node == nullptr) {
      return 0;
    }
    const size_t left = subtreeHeight(node->left);
    const size_t right = subtreeHeight(node->right);
    return (left > right ? left : right) + 1;
  }

  template< class Key, class Value, class Compare >
  void BSTree< Key, Value, Compare >::swap(BSTree& other)
  {
    TreeNodeBase* tmpRoot = fakeRoot_;
    fakeRoot_ = other.fakeRoot_;
    other.fakeRoot_ = tmpRoot;

    const size_t tmpSize = size_;
    size_ = other.size_;
    other.size_ = tmpSize;

    const Compare tmpCompare = compare_;
    compare_ = other.compare_;
    other.compare_ = tmpCompare;
  }
}

#endif
