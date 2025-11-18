#include "imprimirdialog.h"
#include "ui_imprimirdialog.h"

ImprimirDialog::ImprimirDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ImprimirDialog)
{
    ui->setupUi(this);
    if(parent)
        move(parent->geometry().center() - rect().center());
}

ImprimirDialog::~ImprimirDialog()
{
    delete ui;
}
void ImprimirDialog::on_pushButtonCancelar_clicked(){
    close();
}
