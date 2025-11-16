#include "welcomescreen.h"
#include "ui_welcomescreen.h"

WelcomeScreen::WelcomeScreen(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::WelcomeScreen)
{
    ui->setupUi(this);
}

WelcomeScreen::~WelcomeScreen()
{
    delete ui;
}
