#ifndef NEWREGISTRATIONWIDGET_H
#define NEWREGISTRATIONWIDGET_H

#include <QWidget>

namespace Ui {
class NewRegistrationWidget;
}

class NewRegistrationWidget : public QWidget
{
    Q_OBJECT

public:
    explicit NewRegistrationWidget(QWidget *parent = nullptr);
    ~NewRegistrationWidget();

private:
    Ui::NewRegistrationWidget *ui;
};

#endif // NEWREGISTRATIONWIDGET_H
