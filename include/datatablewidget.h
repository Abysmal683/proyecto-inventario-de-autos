#ifndef DATATABLEWIDGET_H
#define DATATABLEWIDGET_H

#include <QWidget>

namespace Ui {
class DataTableWidget;
}

class DataTableWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DataTableWidget(QWidget *parent = nullptr);
    ~DataTableWidget();
signals:
    void goWelcomeWidgetRequested();

private:
    Ui::DataTableWidget *ui;
};

#endif // DATATABLEWIDGET_H
