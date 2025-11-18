#ifndef WELCOMEWIDGET_H
#define WELCOMEWIDGET_H

#include <QWidget>

namespace Ui {
class WelcomeWidget;
}

class WelcomeWidget : public QWidget
{
    Q_OBJECT

public:
    explicit WelcomeWidget(QWidget *parent = nullptr);
    ~WelcomeWidget();

signals:
    void goDataTableWidgetRequested();
    void finishMainWindowRequested();
    void goDataTableWidgetAndResearchRequested();
private:
    Ui::WelcomeWidget *ui;
};

#endif // WELCOMEWIDGET_H
