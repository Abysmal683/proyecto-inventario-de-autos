#include "imprimirscreen.h"
#include "ui_imprimirscreen.h"

ImprimirScreen::ImprimirScreen(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ImprimirScreen)
{
    ui->setupUi(this);
    setWindowModality(Qt::ApplicationModal);
    setAttribute(Qt::WA_DeleteOnClose);
}

ImprimirScreen::~ImprimirScreen()
{
    delete ui;
}
void ImprimirScreen::on_pushButtonCancelar_clicked(){
    close();
}
