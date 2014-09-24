#include "mainwindow.h"
#include <QApplication>
#include <QErrorMessage>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    QErrorMessage::qtHandler();

    return a.exec();
}
