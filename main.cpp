#include "treePainter.h"
#include "avlTree.hpp"
#include "treeLocator.hpp"
#include "seqBinaryTree.hpp"
#include <vector>
#include <string>
#include <iterator>
#include "mainwindow.h"
#include <QApplication>

int main(int argc, char* argv[])
{
    //default golbal parameter
    double radius = 35;
    double height = 1000;
    double width = 1400;
    //run application
    QApplication a(argc, argv);
    MainWindow m(radius, height, width);
    m.show();
    return a.exec();
}
