#include "mainwindow.h"
#include <QApplication>
#include <QFontDatabase>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QFontDatabase::addApplicationFont ("Entypo.ttf");
    MainWindow w;
    w.show();

    return app.exec();
}
