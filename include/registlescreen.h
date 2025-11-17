#ifndef REGISTLESCREEN_H
#define REGISTLESCREEN_H

#include <QWidget>

namespace Ui {
class RegistleScreen;
}

class RegistleScreen : public QWidget
{
    Q_OBJECT

public:
    explicit RegistleScreen(QWidget *parent = nullptr);
    ~RegistleScreen();

private:
    Ui::RegistleScreen *ui;
};

#endif // REGISTLESCREEN_H
