#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "welcomescreen.h"
#include "registlescreen.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Crear widgets (pantallas)
    welcomeScreen = new WelcomeScreen(this);
    registleScreen = new RegistleScreen(this);

    // Agregar pantallas al StackedWidget
    ui->stackedWidget->addWidget(welcomeScreen);
    ui->stackedWidget->addWidget(registleScreen);

    // Señal desde WelcomeScreen → cambiar pantalla a register
    connect(welcomeScreen, &WelcomeScreen::goRegisterRequested,
            this, &MainWindow::goToRegister);
    // Señal desde register → cambiar pantalla a welcome
    connect(registleScreen, &RegistleScreen::goWelcomeRequested,
            this, &MainWindow::goToWelcome);
    // finalizar proceso
    connect(welcomeScreen, &WelcomeScreen::finishMainRequested,
            this, &MainWindow::finish);

    // Mostrar la pantalla inicial
    ui->stackedWidget->setCurrentWidget(welcomeScreen);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::goToRegister(){
    ui->stackedWidget->setCurrentWidget(registleScreen);
}
void MainWindow::goToWelcome(){
    ui->stackedWidget->setCurrentWidget(welcomeScreen);
}
void MainWindow::finish(){
    close();
}

