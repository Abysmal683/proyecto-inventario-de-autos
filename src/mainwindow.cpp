#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "welcomewidget.h"
#include "datatablewidget.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Crear widgets (pantallas)
    welcomeWidget = new WelcomeWidget(this);
    dataTableWidget = new DataTableWidget(this);

    // Agregar pantallas al StackedWidget
    ui->stackedWidget->addWidget(welcomeWidget);
    ui->stackedWidget->addWidget(dataTableWidget);

    // Señal desde WelcomeWidget → cambiar pantalla a datatable
    connect(welcomeWidget, &WelcomeWidget::goDataTableWidgetRequested,
            this, &MainWindow::goToDataTableWidget);
    // Señal desde register → cambiar pantalla a welcome
    connect(dataTableWidget, &DataTableWidget::goWelcomeWidgetRequested,
            this, &MainWindow::goToWelcomeWidget);
    // finalizar proceso
    connect(welcomeWidget, &WelcomeWidget::finishMainWindowRequested,
            this, &MainWindow::finish);

    // Mostrar la pantalla inicial
    ui->stackedWidget->setCurrentWidget(welcomeWidget);
}

MainWindow::~MainWindow()
{
    delete ui;
}
//funciones que realiza las accones por las senales entrantes
void MainWindow::goToDataTableWidget(){
    ui->stackedWidget->setCurrentWidget(dataTableWidget);
}
void MainWindow::goToWelcomeWidget(){
    ui->stackedWidget->setCurrentWidget(welcomeWidget);
}
void MainWindow::finish(){
    close();
}

