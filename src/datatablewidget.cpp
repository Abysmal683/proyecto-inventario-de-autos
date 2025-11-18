#include "datatablewidget.h"
#include "ui_datatablewidget.h"

DataTableWidget::DataTableWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::DataTableWidget)
{
    ui->setupUi(this);
    connect(ui->pushButtonRegresar, &QPushButton::clicked, this, [=](){
        emit goWelcomeWidgetRequested();
    });
}

DataTableWidget::~DataTableWidget()
{
    delete ui;
}
