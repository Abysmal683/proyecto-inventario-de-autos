#include "welcomescreen.h"
#include "ui_welcomescreen.h"

WelcomeScreen::WelcomeScreen(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::WelcomeScreen)
{
    ui->setupUi(this);
    connect(ui->push_button_registros, &QPushButton::clicked, this, [=](){
        emit goRegisterRequested();
    });
}

WelcomeScreen::~WelcomeScreen()
{
    delete ui;
}
