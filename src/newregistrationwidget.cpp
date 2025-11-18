#include "newregistrationwidget.h"
#include "ui_newregistrationwidget.h"

NewRegistrationWidget::NewRegistrationWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::NewRegistrationWidget)
{
    ui->setupUi(this);
    connect(ui->pushButtonCancelar, &QPushButton::clicked, this, [=](){
        emit goWelcomeWidgetRequested();
    });
}

NewRegistrationWidget::~NewRegistrationWidget()
{
    delete ui;
}
