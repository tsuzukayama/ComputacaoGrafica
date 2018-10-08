#include "mainwindow.h"
#include <QApplication>
#include <QSurfaceFormat>

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_UseDesktopOpenGL);

    QSurfaceFormat format;

    format.setVersion(4, 1);
    format.setProfile(QSurfaceFormat::CoreProfile);

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
