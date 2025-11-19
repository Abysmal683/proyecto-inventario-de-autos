#include "datatablewidget.h"
#include "ui_datatablewidget.h"
#include "infodialog.h"

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
void DataTableWidget::on_pushButtonDetalles_clicked(){
    InfoDialog infoDialog(this);
    infoDialog.setWindowTitle("Detalles-Extras");
    infoDialog.exec();
}
