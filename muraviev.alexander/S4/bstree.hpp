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
    TreeNode* parent;
    TreeNode* left;
    TreeNode* right;
  };

  template< class Key, class Value, class Compare >
  class BSTree
  {
  public:
    using Node = TreeNode< Key, Value >;
    class const_iterator;

    const_iterator rotateLeft(const_iterator it)
    {
      Node* node = const_cast< Node* >(it.node_);
      if (node == nullptr || node->parent == nullptr || node->parent->right != node) {
        throw std::logic_error("invalid left rotation");
      }
      Node* parent = node->parent;
      Node* subtree = node->left;
      replaceNode(parent, node);
      parent->right = subtree;
      if (subtree != nullptr) {
        subtree->parent = parent;
      }
      node->left = parent;
      parent->parent = node;
      return const_iterator(node);
    }

    const_iterator rotateRight(const_iterator it)
    {
      Node* node = const_cast< Node* >(it.node_);
      if (node == nullptr || node->parent == nullptr || node->parent->left != node) {
        throw std::logic_error("invalid right rotation");
      }
      Node* parent = node->parent;
      Node* subtree = node->right;
      replaceNode(parent, node);
      parent->left = subtree;
      if (subtree != nullptr) {
        subtree->parent = parent;
      }
      node->right = parent;
      parent->parent = node;
      return const_iterator(node);
    }

    const_iterator rotateLargeLeft(const_iterator it);
    const_iterator rotateLargeRight(const_iterator it);
    size_t height() const;
    size_t height(const_iterator it) const;

  private:
    Node* root_;
    size_t size_;
    Compare compare_;

    void replaceNode(Node* oldNode, Node* newNode);
    size_t subtreeHeight(const Node* node) const;
  };
}

#endif
