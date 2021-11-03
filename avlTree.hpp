#ifndef AVLTREE_H
#define AVLTREE_H

#include "linkedBinaryTree.hpp"

template <typename T>
class avlTree : public linkedBinaryTree<T>
{
    using nodePtr = std::unique_ptr<binaryTreeNode<T>>;
public:
    nodePtr& insert(const T, nodePtr&) noexcept override;
    bool erase(const T, nodePtr&) noexcept override;
    friend std::istream& operator>> (std::istream& is, avlTree<T>& t) {
        T theElement;
        is >> theElement;
        if (is)
            t.insert(theElement, t.root());
        return is;
    }
private:
    nodePtr& SingleRotateWithLeft(nodePtr&, nodePtr&) noexcept;
    nodePtr& SingleRotateWithRight(nodePtr&, nodePtr&) noexcept;
    nodePtr& DoubleRotateWithLeft(nodePtr&) noexcept;
    nodePtr& DoubleRotateWithRight(nodePtr&) noexcept;
};

template <typename T>
std::unique_ptr<binaryTreeNode<T>>& avlTree<T>::insert(const T theElement, nodePtr& t) noexcept
{
    if (t == nullptr)
        t = std::make_unique<binaryTreeNode<T>>(theElement);
    else if (theElement < t->element_) { //insert in the leftChild
        t->leftChild_ = std::move(insert(theElement, t->leftChild_));
        if (this->height(t->leftChild_) - this->height(t->rightChild_) > 1) { //unbalanced
            if (theElement < t->leftChild_->element_)
                t = std::move(SingleRotateWithLeft(t, t->leftChild_));
            else
                t = std::move(DoubleRotateWithLeft(t));
        }
    }
    else if (t->element_ < theElement) { //insert in the rightChild
        t->rightChild_ = std::move(insert(theElement, t->rightChild_));
        if (this->height(t->rightChild_) - this->height(t->leftChild_) > 1) {
            if (t->rightChild_->element_ < theElement)
                t = std::move(SingleRotateWithRight(t, t->rightChild_));
            else
                t = std::move(DoubleRotateWithRight(t));
        }
    }
    //else the element was in the AVL tree already
    return t;
}

/*
 *         k2                 k1
 *        .  .               .  .
 *       .    .             .    .
 *      .      .           .      .
 *     k1       Z   ==>   X        k2
 *    .  .                        .  .
 *   .    .                      .    .
 *  X      Y                    Y      Z
 *
 */
template <typename T>
std::unique_ptr<binaryTreeNode<T>>& avlTree<T>::SingleRotateWithLeft(nodePtr& k2, nodePtr& k1) noexcept
{
    //tmp for unique_ptr
    auto tmp {std::move(k1)};
    k2->leftChild_ = std::move(tmp->rightChild_);
    tmp->rightChild_ = std::move(k2);
    k2 = std::move(tmp);
    return k2;
}

/*
 *       k1                   k2
 *      .  .                 .  .
 *     .    .               .    .
 *    .      .             .      .
 *   X        k2    ==>   k1       Z
 *           .  .        .  .
 *          .    .      .    .
 *         Y      Z    X      Y
 *
 */

template <typename T>
std::unique_ptr<binaryTreeNode<T>>& avlTree<T>::SingleRotateWithRight(nodePtr& k1, nodePtr& k2) noexcept
{
    auto tmp {std::move(k2)};
    k1->rightChild_ = std::move(tmp->leftChild_);
    tmp->leftChild_ = std::move(k1);
    k1 = std::move(tmp);
    return k1;
}

/*
 *                 k3                       k2
 *                .  .                     .  .
 *               .    .                   .    .
 *              .      .                 .      .
 *             .        .               .        .
 *           k1          D             k1         k3
 *          .  .                      .  .        . .
 *         .    .             ==>    .    .      .   .
 *        .      .                  .      .    .     .
 *       A        k2               A	    B  C       D
 *               .  .
 *              .    .
 *             .      .
 *            B        C
 */

template <typename T>
std::unique_ptr<binaryTreeNode<T>>& avlTree<T>::DoubleRotateWithLeft(nodePtr& k) noexcept
{
    //k for k3
    k->leftChild_ = std::move(SingleRotateWithRight(k->leftChild_, k->leftChild_->rightChild_));
    return SingleRotateWithLeft(k, k->leftChild_);
}

/*
 *                 k3                       k2
 *                .  .                     .  .
 *               .    .                   .    .
 *              .      .                 .      .
 *             .        .               .        .
 *  		  A         k1             k3         k1
 *     			       .  .           .  .        . .
 *   			      .    .   ==>   .    .      .   .
 *    			     .      .       .      .    .     .
 *   			    k2       D     A	    B  C       D
 *           	   .  .
 *         		  .    .
 *        		 .      .
 *        		B        C
 */

template <typename T>
std::unique_ptr<binaryTreeNode<T>>& avlTree<T>::DoubleRotateWithRight(nodePtr& k) noexcept
{
    //k for k3
    k->rightChild_ = std::move(SingleRotateWithLeft(k->rightChild_, k->rightChild_->leftChild_));
    return SingleRotateWithRight(k, k->rightChild_);
}

template <typename T>
bool avlTree<T>::erase(const T theElement, nodePtr& t) noexcept
{
    if (t == nullptr)
        return false;
    else if (t->element_ == theElement) {
        if (t->leftChild_ != nullptr && t->rightChild_ != nullptr) {
            if (this->height(t->leftChild_) > this->height(t->rightChild_)) { //choose the appropriate branch
                t->element_ = this->findMax(t->leftChild_)->element_; //lift the maximum element(leaf) in the leftChild
                return erase(t->element_, t->leftChild_);
            } else {
                t->element_ = this->findMin(t->rightChild_)->element_; //lift the minimum element(leaf) in the rightChild
                return erase(t->element_, t->rightChild_);
            }
        } else
            t = std::move((t->leftChild_ != nullptr) ? t->leftChild_ : t->rightChild_);
    } else if (theElement < t->element_) {
        erase(theElement, t->leftChild_);
        if (this->height(t->rightChild_) - this->height(t->leftChild_) > 1) { //unbalanced
            if (this->height(t->rightChild_->leftChild_) > this->height(t->rightChild_->rightChild_))
                t = std::move(DoubleRotateWithRight(t));
            else
                t = std::move(SingleRotateWithRight(t, t->rightChild_));
        }
    } else {
        erase(theElement, t->rightChild_);
        if (this->height(t->leftChild_) - this->height(t->rightChild_) > 1) {
            if (this->height(t->leftChild_->rightChild_) > this->height(t->leftChild_->leftChild_))
                t = std::move(DoubleRotateWithLeft(t));
            else
                t = std::move(SingleRotateWithLeft(t, t->leftChild_));
        }
    }
    return true;
}
#endif // AVLTREE_H
