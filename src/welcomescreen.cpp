#include "welcomescreen.h"
#include "ui_welcomescreen.h"
#include "imprimirscreen.h"
WelcomeScreen::WelcomeScreen(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::WelcomeScreen)
{
    ui->setupUi(this);
    connect(ui->pushButtonRegistros, &QPushButton::clicked, this, [=](){
        emit goRegisterRequested();
    });
}
void WelcomeScreen::on_pushButtonImprimir_clicked(){
    ImprimirScreen *imprimirScreen =new ImprimirScreen(this);
    imprimirScreen->show();
}
WelcomeScreen::~WelcomeScreen()
{
    delete ui;
}
