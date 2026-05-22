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

    Value drop(const Key& key)
    {
      Node* removed = find(key);
      if (removed == nullptr) {
        throw std::out_of_range("key not found");
      }

      Value result = removed->value;
      if (removed->left == nullptr) {
        replaceNode(removed, removed->right);
      } else if (removed->right == nullptr) {
        replaceNode(removed, removed->left);
      } else {
        Node* successor = minNode(removed->right);
        if (successor->parent != removed) {
          replaceNode(successor, successor->right);
          successor->right = removed->right;
          successor->right->parent = successor;
        }
        replaceNode(removed, successor);
        successor->left = removed->left;
        successor->left->parent = successor;
      }

      delete removed;
      --size_;
      return result;
    }

    void push(const Key& key, const Value& value);
    Value& get(const Key& key);
    bool contains(const Key& key) const;
    bool empty() const;
    size_t size() const;

  private:
    Node* root_;
    size_t size_;
    Compare compare_;

    Node* find(const Key& key) const;
    Node* minNode(Node* node) const;
    void replaceNode(Node* oldNode, Node* newNode);
  };
}

#endif
