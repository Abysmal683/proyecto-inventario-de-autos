#include "infodialog.h"
#include "ui_infodialog.h"
#include "imprimirdialog.h"
InfoDialog::InfoDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::InfoDialog)
{
    ui->setupUi(this);
    if(parent)
        move(parent->geometry().center() - rect().center());

}

InfoDialog::~InfoDialog()
{
    delete ui;
}
void InfoDialog::on_pushButtonRegresar_clicked(){
    close();
}
void InfoDialog::on_pushButtonImprimir_clicked(){
    ImprimirDialog imprimirDialog(this);
    imprimirDialog.setWindowTitle("Imprimir");
    imprimirDialog.exec();
}
