#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "welcomescreen.h"
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //agregamos la pantalla de bienvenida
    WelcomeScreen *welcomeScreen = new WelcomeScreen(this);
    //mostrar en el centralwidget
    setCentralWidget(welcomeScreen);
}

MainWindow::~MainWindow()
{
    delete ui;
}
