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
    connect(ui->pushButtonFinalizar,&QPushButton::clicked,this,[=](){
       emit finishMainWindowRequested();
    });
}
void WelcomeWidget::on_pushButtonImprimir_clicked(){
    ImprimirDialog imprimirDialog(this);
    imprimirDialog.exec();
}
WelcomeWidget::~WelcomeWidget()
{
    delete ui;
}
