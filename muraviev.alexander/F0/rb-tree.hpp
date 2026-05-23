#ifndef RB_TREE_HPP
#define RB_TREE_HPP

#include <cstddef>
#include <stdexcept>

namespace muraviev
{
  template< class T >
  struct Less { bool operator()(const T& left, const T& right) const { return left < right; } };
  enum RBColor { RED, BLACK };

  template< class Key, class Value >
  struct RBNode
  {
    Key key;
    Value value;
    RBNode* parent;
    RBNode* left;
    RBNode* right;
    RBColor color;
    RBNode(const Key& nodeKey, const Value& nodeValue): key(nodeKey), value(nodeValue), parent(0), left(0), right(0), color(RED) {}
  };

  template< class Key, class Value, class Compare >
  class RBTree
  {
  public:
    RBTree(): root_(0), size_(0) {}
    void push(const Key& key, const Value& value)
    {
      RBNode< Key, Value >* parent = 0;
      RBNode< Key, Value >* current = root_;
      while (current != 0) {
        parent = current;
        if (compare_(key, current->key)) {
          current = current->left;
        } else if (compare_(current->key, key)) {
          current = current->right;
        } else {
          current->value = value;
          return;
        }
      }
      RBNode< Key, Value >* node = new RBNode< Key, Value >(key, value);
      node->parent = parent;
      if (parent == 0) {
        root_ = node;
      } else if (compare_(key, parent->key)) {
        parent->left = node;
      } else {
        parent->right = node;
      }
      root_->color = BLACK;
      ++size_;
    }
    Value& get(const Key& key)
    {
      RBNode< Key, Value >* node = findNode(key);
      if (node == 0) { throw std::out_of_range("key not found"); }
      return node->value;
    }
    bool empty() const { return size_ == 0; }
    size_t size() const { return size_; }
    bool contains(const Key& key) const { return findNode(key) != 0; }
    bool valid() const { return root_ == 0 || root_->color == BLACK; }
  private:
    RBNode< Key, Value >* root_;
    size_t size_;
    Compare compare_;
    RBNode< Key, Value >* findNode(const Key& key) const
    {
      RBNode< Key, Value >* current = root_;
      while (current != 0) {
        if (compare_(key, current->key)) { current = current->left; }
        else if (compare_(current->key, key)) { current = current->right; }
        else { return current; }
      }
      return 0;
    }
  };
}

#endif
