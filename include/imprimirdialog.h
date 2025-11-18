#ifndef IMPRIMIRDIALOG_H
#define IMPRIMIRDIALOG_H

#include <QDialog>

namespace Ui {
class ImprimirDialog;
}

class ImprimirDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ImprimirDialog(QWidget *parent = nullptr);
    ~ImprimirDialog();

private slots:
    void on_pushButtonCancelar_clicked();

private:
    Ui::ImprimirDialog *ui;
};

#endif // IMPRIMIRDIALOG_H
