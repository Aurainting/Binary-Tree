#ifndef BINARYTREENODE_H
#define BINARYTREENODE_H

#include <memory>

template <typename T>
class binaryTreeNode
{
    using nodePtr = std::unique_ptr<binaryTreeNode<T>>;
public:
    T element_;
    nodePtr leftChild_;
    nodePtr rightChild_;
    binaryTreeNode() : element_(), leftChild_(), rightChild_() { }
    binaryTreeNode(const T& theElement) : element_(theElement), leftChild_(), rightChild_() { }
    binaryTreeNode(const nodePtr&);
};

template <typename T>
binaryTreeNode<T>::binaryTreeNode(const nodePtr& rhs)
{
    //deep copy
    if (rhs == nullptr)
        binaryTreeNode();
    else {
        element_ = rhs->element_;
        if (rhs->leftChild_ != nullptr) {
            leftChild_ = std::make_unique<binaryTreeNode<T>>(rhs->leftChild_);
        }
        if (rhs->rightChild_ != nullptr) {
            rightChild_ = std::make_unique<binaryTreeNode<T>>(rhs->rightChild_);
        }
    }
};

#endif // BINARYTREENODE_H
