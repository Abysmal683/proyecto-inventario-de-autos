#include "welcomewidget.h"
#include "ui_welcomewidget.h"
#include "imprimirdialog.h"
WelcomeWidget::WelcomeWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::WelcomeWidget)
{
    ui->setupUi(this);
    connect(ui->pushButtonRegistros, &QPushButton::clicked, this, [=](){
        emit goDataTableWidgetRequested();
    });
    connect(ui->pushButtonBuscar, &QPushButton::clicked, this, [=](){
        emit goDataTableWidgetAndResearchRequested();
    });
    connect(ui->pushButtonFinalizar,&QPushButton::clicked,this,[=](){
       emit finishMainWindowRequested();
    });
    connect(ui->pushButtonNuevoRegistro,&QPushButton::clicked,this,[=](){
       emit goNewRegistrationWidgetRequested();
    });
}
WelcomeWidget::~WelcomeWidget()
{
    delete ui;
}
