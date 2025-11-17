#ifndef IMPRIMIRSCREEN_H
#define IMPRIMIRSCREEN_H

#include <QWidget>

namespace Ui {
class ImprimirScreen;
}

class ImprimirScreen : public QWidget
{
    Q_OBJECT

public:
    explicit ImprimirScreen(QWidget *parent = nullptr);
    ~ImprimirScreen();

private slots:
    void on_pushButtonCancelar_clicked();

private:
    Ui::ImprimirScreen *ui;
};

#endif // IMPRIMIRSCREEN_H
