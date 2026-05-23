#ifndef RB_TREE_HPP
#define RB_TREE_HPP

#include <cstddef>
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
    void deleteSubtree(RBNodeBase* node)
    {
      if (node == 0) { return; }
      deleteSubtree(node->left);
      deleteSubtree(node->right);
      delete static_cast< Node* >(node);
    }
  };
}

#endif
