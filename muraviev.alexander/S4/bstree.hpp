#ifndef BSTREE_HPP
#define BSTREE_HPP

#include <cstddef>

namespace muraviev
{
  template< class Key, class Value >
  struct TreeNode
  {
    Key key;
    Value value;
    TreeNode* parent;
    TreeNode* left;
    TreeNode* right;
  };

  template< class Key, class Value, class Compare >
  class BSTree
  {
  public:
    using Node = TreeNode< Key, Value >;

    BSTree(const BSTree& other):
      root_(clone(other.root_, nullptr)),
      size_(other.size_),
      compare_(other.compare_)
    {}

    BSTree& operator=(const BSTree& other)
    {
      if (this != &other) {
        BSTree copy(other);
        swap(copy);
      }
      return *this;
    }

    void clear()
    {
      deleteSubtree(root_);
      root_ = nullptr;
      size_ = 0;
    }

    void push(const Key& key, const Value& value);
    Value& get(const Key& key);
    Value drop(const Key& key);
    bool empty() const;
    size_t size() const;

  private:
    Node* root_;
    size_t size_;
    Compare compare_;

    Node* clone(const Node* node, Node* parent);
    void deleteSubtree(Node* node);
    void swap(BSTree& other);
  };
}

#endif
