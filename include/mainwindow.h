#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class WelcomeScreen;
class RegistleScreen;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    WelcomeScreen *welcomeScreen;
    RegistleScreen *registleScreen;
private slots:
    void goToRegister();
};

#endif // MAINWINDOW_H
