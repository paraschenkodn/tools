#include <QApplication>
#include "mainwindow.h"

int main( int argc, char* argv[] )
{
    // QFileInfo(argv[0]).dir().path()  //app-path
    QApplication a( argc, argv );

    MainWindow w;
    w.loadPlugins();
    w.show();

return a.exec();
}
