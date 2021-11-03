#ifndef SEQBINARYTREE_HPP
#define SEQBINARYTREE_HPP

#include <functional>
#include <algorithm>
#include <vector>
#include <deque>
#include <memory>
#include "binaryTreeNode.hpp"
#include "binaryTree.hpp"

template <typename T>
class seqBinaryTree : public binaryTree<binaryTreeNode<T>>
{
    using node = binaryTreeNode<T>;
    using nodePtr = std::unique_ptr<node>;
    using VisitFunc = std::function<void(nodePtr&)>;
public:
    seqBinaryTree() = default;
    seqBinaryTree(const std::vector<T>& v) noexcept
        : vec_()
    {
        auto nope = T(); //no element
        for (const auto& i : v) {
            if (i == nope)
                vec_.emplace_back(nullptr);
            else
                vec_.emplace_back(std::make_unique<node>(i));
        }
    }
    ~seqBinaryTree() {vec_.clear();}
    inline bool empty() const override {return vec_.empty();}
    inline int height() noexcept override {return height_(0);}
    inline int nodeCount() noexcept override {return static_cast<int>(vec_.size() - std::count(vec_.begin(), vec_.end(), nullptr));}
    void preOrder(const VisitFunc& theVisit) noexcept
    {
        auto size {vec_.size()};
        /*
         *	use stack to get the preOrder of the index (begin from 0)
         *	if (i) is the root, then (2i+1) is leftChild and the (2i+2) is rightChild
         */
        std::deque<decltype (size)> stack;
        stack.emplace_back(0);
        while (!stack.empty()) {
            auto tmp {stack.back()};
            stack.pop_back();
            if (vec_[tmp] != nullptr)
                theVisit(vec_[tmp]);
            if (++ ++(tmp <<= 1)< size) //push on rightChild
                stack.emplace_back(tmp);
            if (--tmp < size) //push on leftChild
                stack.emplace_back(tmp);
        }
    }
private:
    std::vector<nodePtr> vec_;
    int height_(typename std::vector<nodePtr>::size_type) noexcept;
};

template <typename T>
int seqBinaryTree<T>::height_(typename std::vector<nodePtr>::size_type i) noexcept
{
    if (i >= vec_.size())
        return 0;
    if (vec_[i] == nullptr)
        return 0;
    else {
        auto hl {height_( (i << 1) + 1 )}; //height for leftChild
        auto hr {height_( (i << 1) + 2 )}; //height for rightChild
        return hl > hr ? ++hl : ++hr;
    }
}

#endif // SEQBINARYTREE_HPP
