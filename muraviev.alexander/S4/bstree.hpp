#ifndef BSTREE_HPP
#define BSTREE_HPP

#include <cstddef>
#include <stdexcept>

namespace muraviev
{
  template< class Key, class Value >
  struct TreeNode
  {
    Key key;
    Value value;
    TreeNode* left;
    TreeNode* right;

    TreeNode(const Key& nodeKey, const Value& nodeValue):
      key(nodeKey),
      value(nodeValue),
      left(nullptr),
      right(nullptr)
    {}
  };

  template< class Key, class Value, class Compare >
  class BSTree
  {
  public:
    BSTree():
      root_(nullptr),
      size_(0)
    {}

    ~BSTree()
    {
      clear(root_);
    }

    void push(const Key& key, const Value& value)
    {
      TreeNode< Key, Value >** current = &root_;
      while (*current != nullptr) {
        if (compare_(key, (*current)->key)) {
          current = &((*current)->left);
        } else if (compare_((*current)->key, key)) {
          current = &((*current)->right);
        } else {
          (*current)->value = value;
          return;
        }
      }
      *current = new TreeNode< Key, Value >(key, value);
      ++size_;
    }

    Value& get(const Key& key)
    {
      TreeNode< Key, Value >* node = find(key);
      if (node == nullptr) {
        throw std::out_of_range("key not found");
      }
      return node->value;
    }

    bool contains(const Key& key) const
    {
      return find(key) != nullptr;
    }

    bool empty() const
    {
      return size_ == 0;
    }

    size_t size() const
    {
      return size_;
    }

  private:
    TreeNode< Key, Value >* root_;
    size_t size_;
    Compare compare_;

    TreeNode< Key, Value >* find(const Key& key) const
    {
      TreeNode< Key, Value >* current = root_;
      while (current != nullptr) {
        if (compare_(key, current->key)) {
          current = current->left;
        } else if (compare_(current->key, key)) {
          current = current->right;
        } else {
          return current;
        }
      }
      return nullptr;
    }

    void clear(TreeNode< Key, Value >* node)
    {
      if (node == nullptr) {
        return;
      }
      clear(node->left);
      clear(node->right);
      delete node;
    }
  };
}

#endif
