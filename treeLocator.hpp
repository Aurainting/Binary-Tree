#ifndef TREELOCATOR_H
#define TREELOCATOR_H

#include <vector>
#include <string>
#include <deque>
#include <cmath>
#include <utility>
#include <numeric>
#include <QPoint>
#include <QLineF>
#include "linkedBinaryTree.hpp"

template <typename T>
class treeLocate
{
    using nodePtr = std::unique_ptr<binaryTreeNode<T>>;
    using size_type = std::vector<QPointF>::size_type;
public:
    explicit treeLocate(const linkedBinaryTree<T>& tree, double radius, double height, double width, double beginFrom = 0)
        : radius_(radius)
        , height_(height - 120)
        , width_(width)
        , beginFrom_(beginFrom)
        , points_()
        , lines_()
        , passerby_()
        , path_()
        , tree_(tree)
    {
        if (!tree_.empty())
            prepare();
    }
    inline std::vector<QPointF> points() noexcept {return points_;}
    inline std::vector<QLineF> lines() noexcept {return lines_;}
    std::vector<size_type> findPath(const T theElement) noexcept
    {
        path_.clear();
        passerby_.clear();
        path_.resize(0);
        passerby_.resize(0);
        findPasserby(theElement, tree_.root());
        return find();
    }
private:
    double radius_;
    double height_;
    double width_;
    double beginFrom_;
    std::vector<QPointF> points_;
    std::vector<QLineF> lines_;
    std::vector<T> passerby_;
    std::vector<size_type> path_;
    linkedBinaryTree<T> tree_;
    void prepare() noexcept;
    void findPasserby(const T, const nodePtr&) noexcept;
    std::vector<size_type> find() noexcept;
};

template <typename T>
void treeLocate<T>::prepare() noexcept
{
    /*
     *	show for all the screen
     *	dHeight (delta height): height between levels
     */
    const double dHeight = height_ / tree_.height();
    points_.emplace_back(beginFrom_ + width_ / 2, 0);//initialize for the root
    int levelNo = 0;//current level No. (begin from 0)
    int levelCnt = 0;//current level node count
    const std::vector<int> levelInfo = tree_.levelInfo();
    double dist = width_ / ((1 << levelNo) + 1);//current level distance
    double lowDist = width_ / ((1 << (levelNo + 1)) + 1);//lower level distance
    tree_.levelOrder([this, &dHeight, &levelNo, &levelCnt, &levelInfo, &dist, &lowDist](const auto& p)
    {
        ++levelCnt;
        if (p->leftChild_ != nullptr){
            int index = 0;
            if (levelNo != 0)
                index = std::accumulate(levelInfo.begin(), levelInfo.begin() + levelNo, 0) //count all higher level nodes
                        + levelCnt //current level node(s)
                        - 1; //-1 for index
            double parentX = points_[index].x();
            double parentY = points_[index].y();
            //add points
            unsigned upLevelTimes = static_cast<unsigned>(std::round((parentX - beginFrom_) / dist));
            unsigned lowLevelLeftTimes = (upLevelTimes << 1) - 1;

            double childX = beginFrom_ + lowLevelLeftTimes * lowDist;
            double childY = dHeight * (levelNo + 1);
            double otherX = (*(points_.cend() - 1)).x();
            double otherY = (*(points_.cend() - 1)).y();
            if (otherY == childY) {
                if (childX - otherX < 2 * radius_)
                    childX = otherX + 2 * radius_;
            }


            points_.emplace_back(childX, childY);
            //add lines
            double distance = parentX - childX;
            double hypotenuse = std::sqrt(std::pow(distance, 2) + std::pow(dHeight - radius_, 2));
            double x = distance * radius_ / hypotenuse;
            double y = (dHeight - radius_) * radius_ / hypotenuse;
            lines_.emplace_back(parentX - x, parentY + y,
                        childX, parentY + dHeight - radius_);
        }
        if (p->rightChild_ != nullptr){
            int index = 0;
            if (levelNo != 0)
                index = std::accumulate(levelInfo.begin(), levelInfo.begin() + levelNo, 0) //count all higher level nodes
                        + levelCnt //current level node(s)
                        - 1; //-1 for index
            double parentX = points_[index].x();
            double parentY = points_[index].y();
            //add points
            unsigned upLevelTimes = static_cast<unsigned>(std::round((parentX  - beginFrom_) / dist));
            unsigned lowLevelRightTimes = upLevelTimes << 1;

            double childX = beginFrom_ + lowLevelRightTimes * lowDist;
            double childY = dHeight * (levelNo + 1);
            double otherX = (*(points_.cend() - 1)).x();
            double otherY = (*(points_.cend() - 1)).y();
            if (otherY == childY) {
                if (childX - otherX < 2 * radius_)
                    childX = otherX + 2 * radius_;
            }


            points_.emplace_back(childX, childY);
            //add lines
            double distance = childX - parentX;
            double hypotenuse = std::sqrt(std::pow(distance, 2) + std::pow(dHeight - radius_, 2));
            double x = distance * radius_ / hypotenuse;
            double y = (dHeight - radius_) * radius_ / hypotenuse;
            lines_.emplace_back(parentX + x, parentY + y,
                        childX, parentY + dHeight - radius_);
        }
        if (levelCnt == levelInfo[levelNo]){//current level is done
            ++levelNo;
            dist = width_ / ((1 << levelNo) + 1);
            lowDist = width_ / ((1 << (levelNo + 1)) + 1);
            levelCnt = 0;
        }
    });
}

template <typename T>
void treeLocate<T>::findPasserby(const T theElement, const std::unique_ptr<binaryTreeNode<T>>& t) noexcept
{
    auto tmp {t->element_};
    passerby_.emplace_back(tmp);
    if (tmp < theElement)
        findPasserby(theElement, t->rightChild_);
    else if (tmp > theElement)
        findPasserby(theElement, t->leftChild_);
}

template <typename T>
std::vector<std::vector<QPointF>::size_type> treeLocate<T>::find() noexcept
{
    size_type cnt {};
    size_type index {}; //index of passerby_
    tree_.levelOrder([this, &cnt, &index](const auto& p)
    {
        if (path_.size() != passerby_.size())
            if (p->element_ == passerby_[index]) {
                ++index;
                path_.emplace_back(cnt);
            }
        ++cnt;
    });
    return path_;
}
#endif // TREELOCATOR_H
