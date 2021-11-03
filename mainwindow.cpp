#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(double radius, double height, double width, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    radius_(radius),
    height_(height),
    width_(width),
    tPainter_(nullptr)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    tPainter_ = nullptr;
    delete ui;
}

void MainWindow::on_toAVL_clicked()
{
    tPainter_ = std::make_unique<treePainter>(radius_, height_, width_);
    this->close();
    tPainter_->show();
}
