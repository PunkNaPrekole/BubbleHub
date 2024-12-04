#include "mainwindow.h"

#include <QApplication>
#include <QIcon>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(":/icon.png"));
    MainWindow w;
    w.setWindowTitle("BubbleHub");
    w.showMaximized();
    return a.exec();
}
