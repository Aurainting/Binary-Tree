#ifndef BINARYTREE_H
#define BINARYTREE_H

template <typename T>
class binaryTree
{
public:
    virtual ~binaryTree() { };
    virtual bool empty() const = 0;
    virtual int height() noexcept = 0;
    virtual int nodeCount() noexcept = 0;
};

#endif // BINARYTREE_H
