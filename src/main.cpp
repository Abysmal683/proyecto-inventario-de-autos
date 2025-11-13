#include <QApplication>
#include <QMainWindow>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QMainWindow mainWindow;
    mainWindow.setWindowTitle("Inventario-de-Autos");
    mainWindow.resize(600, 400);
    mainWindow.show();

    return app.exec();
}
