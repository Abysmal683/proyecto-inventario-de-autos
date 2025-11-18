#include <QApplication>
#include "mainwindow.h"
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    MainWindow mainWindow;
    mainWindow.setWindowTitle("Inventario de Autos");
    mainWindow.resize(600, 400);
    mainWindow.show();
    return app.exec();
}
