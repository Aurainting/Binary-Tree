#ifndef LINKEDBINARYTREE_HPP
#define LINKEDBINARYTREE_HPP

#include <functional>
#include <vector>
#include <deque>
#include <iterator>
#include <algorithm>
#include <iostream>
#include <utility>
#include "binaryTree.hpp"
#include "binaryTreeNode.hpp"

using std::vector;

template <typename T>
class linkedBinaryTree : public binaryTree<binaryTreeNode<T>>
{
    using node = binaryTreeNode<T>;
    using nodePtr = std::unique_ptr<node>;
    using VisitFunc = std::function<void(nodePtr&)>;

public:
    linkedBinaryTree() = default;
    linkedBinaryTree(const linkedBinaryTree<T>&);
    ~linkedBinaryTree();
    inline bool empty() const noexcept override {return root_ == nullptr;}
    inline int height() noexcept override {return height_(root_);}
    int height(const nodePtr& r) noexcept {return height_(r);}
    int nodeCount() noexcept override;
    bool find(const T theElement) noexcept {return find_(theElement, root_);}
    virtual nodePtr& insert(const T, nodePtr&) noexcept;
    virtual bool erase(const T, nodePtr&) noexcept;
    inline nodePtr& root() noexcept {return root_;}
    inline T value(nodePtr& theNode) const noexcept {return theNode->element_;}
    inline void setValue(nodePtr& theNode, T theElement) noexcept {theNode->element_ = theElement;}
    vector<int> levelInfo() noexcept;
    void preOrder(const VisitFunc& theVisit) noexcept
    {
        visit_ = theVisit;
        preOrder_(root_);
    }
    void inOrder(const VisitFunc& theVisit) noexcept
    {
        visit_ = theVisit;
        inOrder_(root_);
    }
    void postOrder(const VisitFunc& theVisit) noexcept
    {
        visit_ = theVisit;
        postOrder_(root_);
    }
    void levelOrder(const VisitFunc& theVisit) noexcept
    {
        visit_ = theVisit;
        levelOrder_(root_);
    }
    inline nodePtr& findMax(nodePtr& t) const noexcept {return (t->rightChild_ == nullptr) ? t : findMax(t->rightChild_);
                               }
    inline nodePtr& findMin(nodePtr& t) const noexcept {return (t->leftChild_ == nullptr) ? t : findMin(t->leftChild_);
                               }
    friend std::istream& operator>> (std::istream& is, linkedBinaryTree<T>& t) {
        T theElement;
        is >> theElement;
        if (is)
            t.insert(theElement, t.root());
        return is;
    }
private:
    nodePtr root_;
    VisitFunc visit_;
    vector<T> preSeq_;
    vector<T> inSeq_;
    vector<T> postSeq_;
    vector<int> levelInfo_;
    int height_(const nodePtr&) noexcept;
    bool find_(const T, const nodePtr&) noexcept;
    void preOrder_(nodePtr&) noexcept;
    void inOrder_(nodePtr&) noexcept;
    void postOrder_(nodePtr&) noexcept;
    void levelOrder_(const nodePtr&) noexcept;
};

template <typename T>
linkedBinaryTree<T>::linkedBinaryTree(const linkedBinaryTree<T>& rhs)
{
    //deep copy
    root_ = std::make_unique<node>(rhs.root_);
    visit_ = rhs.visit_;
    preSeq_.clear();
    preSeq_.reserve(rhs.preSeq_.size());
    std::copy(rhs.preSeq_.cbegin(), rhs.preSeq_.cend(), preSeq_.begin());
    inSeq_.clear();
    inSeq_.reserve(rhs.inSeq_.size());
    std::copy(rhs.inSeq_.cbegin(), rhs.inSeq_.cend(), inSeq_.begin());
    postSeq_.clear();
    postSeq_.reserve(rhs.postSeq_.size());
    std::copy(rhs.postSeq_.cbegin(), rhs.postSeq_.cend(), postSeq_.begin());
    levelInfo_.clear();
    levelInfo_.reserve(rhs.levelInfo_.size());
    std::copy(rhs.levelInfo_.cbegin(), rhs.levelInfo_.cend(), levelInfo_.begin());
}


template <typename T>
linkedBinaryTree<T>::~linkedBinaryTree()
{
    root_ = nullptr;//unique_ptr
}

template <typename T>
int linkedBinaryTree<T>::height_(const nodePtr& t) noexcept
{
    if (t == nullptr)
        return 0;
    else {
        auto hl {height_(t->leftChild_)}; //height for the left
        auto hr {height_(t->rightChild_)}; //height for the right
        return hl > hr ? ++hl : ++hr;
    }
}

template <typename T>
int linkedBinaryTree<T>::nodeCount() noexcept
{
    int cnt {};
    this->postOrder([&cnt](nodePtr& t)
    {
        if (t != nullptr)
            ++cnt;
    });
    return cnt;
}

template <typename T>
bool linkedBinaryTree<T>::find_(const T theElement, const nodePtr& t) noexcept
{
    if (t == nullptr) return false;
    if (t->element_ == theElement)
        return true;
    else if (t->element_ < theElement)
        return find_(theElement, t->rightChild_);
    else
        return find_(theElement, t->leftChild_);
}

template <typename T>
std::unique_ptr<binaryTreeNode<T>>& linkedBinaryTree<T>::insert(const T theElement, nodePtr& t) noexcept
{
    if (t == nullptr){
        t = std::make_unique<node>(theElement);
        return t;
    }
    else if (theElement < t->element_)
        return insert(theElement, t->leftChild_);
    else if (theElement > t->element_)
        return insert(theElement, t->rightChild_);
    else
        return t;
}

template <typename T>
bool linkedBinaryTree<T>::erase(const T theElement, nodePtr& t) noexcept
{
    if (t == nullptr)
        return false;
    else {
        if (theElement < t->element_)
            return erase(theElement, t->leftChild_);
        else if (theElement > t->element_)
            return erase(theElement, t->rightChild_);
        else if (t->leftChild_ != nullptr && t->rightChild_ != nullptr) { //not the leaf
            t->element_ = this->findMin(t->rightChild_)->element_; //lift the minimum element(leaf) from its rightChild
            return erase(t->element_, t->rightChild_); //erase the leaf
        } else { //one of its child is nullptr
            t = std::move((t->leftChild_ != nullptr) ? t->leftChild_ : t->rightChild_);
            return true;
        }
    }
}

template <typename T>
void linkedBinaryTree<T>::preOrder_(nodePtr& t) noexcept
{
    if (t != nullptr) {
        linkedBinaryTree<T>::visit_(t);
        preOrder_(t->leftChild_);
        preOrder_(t->rightChild_);
    }
};

template <typename T>
void linkedBinaryTree<T>::inOrder_(nodePtr& t) noexcept
{
    if (t != nullptr) {
        inOrder_(t->leftChild_);
        linkedBinaryTree<T>::visit_(t);
        inOrder_(t->rightChild_);
    }
}

template <typename T>
void linkedBinaryTree<T>::postOrder_(nodePtr& t) noexcept
{
    if (t != nullptr) {
        postOrder_(t->leftChild_);
        postOrder_(t->rightChild_);
        linkedBinaryTree<T>::visit_(t);
    }
}

//read method
template <typename T>
void linkedBinaryTree<T>::levelOrder_(const nodePtr& t) noexcept
{
    if (t) {
        std::deque<nodePtr> queue;
        //tmp use for move
        auto tmp = std::make_unique<node>(t);
        while (tmp != nullptr) {
            visit_(tmp);
            if (tmp->leftChild_ != nullptr)
                queue.emplace_back(std::move(tmp->leftChild_));
            if (tmp->rightChild_ != nullptr)
                queue.emplace_back(std::move(tmp->rightChild_));
            if (!queue.empty()) {
                tmp = std::move(queue.front());
                queue.pop_front();
            } else tmp = nullptr;
        }
    }
}

template <typename T>
vector<int> linkedBinaryTree<T>::levelInfo() noexcept
{
    //initialize levelInfo
    levelInfo_.clear();
    levelInfo_.resize(this->height());
    std::fill(levelInfo_.begin(), levelInfo_.end(), 0);
    //initialize queue
    std::deque<std::pair<nodePtr, int>> queue;
    //copyRoot use for move
    auto copyRoot {std::make_unique<node>(root_)};
    if (copyRoot != nullptr) {
        queue.emplace_back(std::make_pair(std::move(copyRoot), 1));
        while (!queue.empty()) {
            auto tmpPtr {std::move(queue.front().first)};
            auto levelNo {queue.front().second};//current node's level No.
            ++levelInfo_[levelNo - 1];
            queue.pop_front();
            if (tmpPtr->leftChild_ != nullptr)
                queue.emplace_back(std::make_pair(std::move(tmpPtr->leftChild_), levelNo + 1));
            if (tmpPtr->rightChild_ != nullptr)
                queue.emplace_back(std::make_pair(std::move(tmpPtr->rightChild_), levelNo + 1));
        }
    }
    return levelInfo_;
}

#endif // LINKEDBINARYTREE_HPP
