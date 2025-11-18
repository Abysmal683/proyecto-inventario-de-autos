#ifndef INFODIALOG_H
#define INFODIALOG_H

#include <QWidget>

namespace Ui {
class InfoDialog;
}

class InfoDialog : public QWidget
{
    Q_OBJECT

public:
    explicit InfoDialog(QWidget *parent = nullptr);
    ~InfoDialog();

private:
    Ui::InfoDialog *ui;
};

#endif // INFODIALOG_H
