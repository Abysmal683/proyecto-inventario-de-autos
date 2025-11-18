#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class WelcomeWidget;
class DataTableWidget;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    WelcomeWidget *welcomeWidget;
    DataTableWidget *dataTableWidget;
private slots:
    void goToDataTableWidget();
    void finish();
    void goToWelcomeWidget();
    void goToDataTableWidgetAndResearch();

};

#endif // MAINWINDOW_H
