#include "imprimirscreen.h"
#include "ui_imprimirscreen.h"

ImprimirScreen::ImprimirScreen(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ImprimirScreen)
{
    ui->setupUi(this);
    if(parent)
        move(parent->geometry().center() - rect().center());
}

ImprimirScreen::~ImprimirScreen()
{
    delete ui;
}
void ImprimirScreen::on_pushButtonCancelar_clicked(){
    close();
}
