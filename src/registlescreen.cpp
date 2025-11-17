#include "registlescreen.h"
#include "ui_registlescreen.h"

RegistleScreen::RegistleScreen(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::RegistleScreen)
{
    ui->setupUi(this);
    connect(ui->pushButtonRegresar, &QPushButton::clicked, this, [=](){
        emit goWelcomeRequested();
    });
}

RegistleScreen::~RegistleScreen()
{
    delete ui;
}
