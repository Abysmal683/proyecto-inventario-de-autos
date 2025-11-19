#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "welcomewidget.h"
#include "datatablewidget.h"
#include "newregistrationwidget.h"
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Crear widgets (pantallas)
    welcomeWidget = new WelcomeWidget(this);
    dataTableWidget = new DataTableWidget(this);
    newRegistrationWidget = new NewRegistrationWidget(this);
    // Agregar pantallas al StackedWidget
    ui->stackedWidgetPantallas->addWidget(welcomeWidget);
    ui->stackedWidgetPantallas->addWidget(dataTableWidget);
    ui->stackedWidgetPantallas->addWidget(newRegistrationWidget);
    // Señal desde WelcomeWidget → cambiar pantalla a datatable
    connect(welcomeWidget, &WelcomeWidget::goDataTableWidgetRequested,
            this, &MainWindow::goToDataTableWidget);
    connect(welcomeWidget, &WelcomeWidget::goDataTableWidgetAndResearchRequested,
            this, &MainWindow::goToDataTableWidgetAndResearch);
    connect(welcomeWidget, &WelcomeWidget::goNewRegistrationWidgetRequested,
            this, &MainWindow::goToNewRegistrationWidget);
    // Señal desde register → cambiar pantalla a welcome
    connect(dataTableWidget, &DataTableWidget::goWelcomeWidgetRequested,
            this, &MainWindow::goToWelcomeWidget);

    // finalizar proceso
    connect(welcomeWidget, &WelcomeWidget::finishMainWindowRequested,
            this, &MainWindow::finish);
    connect(newRegistrationWidget, &NewRegistrationWidget::goWelcomeWidgetRequested,
            this, &MainWindow::goToWelcomeWidget);
    // Mostrar la pantalla inicial
    ui->stackedWidgetPantallas->setCurrentWidget(welcomeWidget);
    resize(600,400);
}

MainWindow::~MainWindow()
{
    delete ui;
}
//funciones que realiza las accones por las senales entrantes
void MainWindow::goToDataTableWidget(){
    ui->stackedWidgetPantallas->setCurrentWidget(dataTableWidget);
}
void MainWindow::goToDataTableWidgetAndResearch(){
// ui->stackedWidgetPantallas->setCurrentWidget(dataTableWidget);
    goToDataTableWidget();
}
void MainWindow::goToWelcomeWidget(){
    ui->stackedWidgetPantallas->setCurrentWidget(welcomeWidget);
}
void MainWindow::goToNewRegistrationWidget(){
    ui->stackedWidgetPantallas->setCurrentWidget(newRegistrationWidget);
}
void MainWindow::finish(){
    close();
}

