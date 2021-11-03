#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <memory>
#include "treePainter.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(double, double, double, QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_toAVL_clicked();

private:
    Ui::MainWindow *ui;
    double radius_;
    double height_;
    double width_;
    std::unique_ptr<treePainter> tPainter_;
};

#endif // MAINWINDOW_H
