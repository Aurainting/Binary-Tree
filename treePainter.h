#ifndef TREEPAINTER_H
#define TREEPAINTER_H

#include <memory>
#include <numeric>
#include <QWidget>
#include <QPainter>
#include <QTimer>
#include "treeLocator.hpp"
#include "avlTree.hpp"

namespace Ui {
class treePainter;
}

class treePainter : public QWidget
{
    Q_OBJECT
    using size_type = std::vector<QPointF>::size_type;
public:
    explicit treePainter(double, double, double, QWidget *parent = nullptr);
    QString info(QString&, const std::unique_ptr<avlTree<int>>&) noexcept;
    ~treePainter();
private slots:
    void on_insertNode_clicked();
    void on_eraseNode_clicked();
    void on_findNode_clicked();
protected:
    void paintEvent(QPaintEvent* event) Q_DECL_OVERRIDE;
private:
    Ui::treePainter *ui;
    double radius_;
    double height_;
    double width_;
    size_type target_;
    QTimer* timer_;
    std::vector<size_type> targetPath_;
    std::vector<QPointF> points_;
    std::vector<QLineF> lines_;
    std::vector<int> contents_;
    QString oldInfo_;
    std::unique_ptr<avlTree<int>> oldTree_;
    QString newInfo_;
    std::unique_ptr<avlTree<int>> newTree_;
    std::unique_ptr<treeLocate<int>> oldLocator_;
    std::unique_ptr<treeLocate<int>> newLocator_;
};

#endif // TREEPAINTER_H
