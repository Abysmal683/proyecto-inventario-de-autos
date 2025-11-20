#ifndef DATATABLEWIDGET_H
#define DATATABLEWIDGET_H

#include <QWidget>
#include <QSqlQueryModel>
namespace Ui {
class DataTableWidget;
}

class DataTableWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DataTableWidget(QWidget *parent = nullptr);
    ~DataTableWidget();
    void setFilters(const QVariantMap &filtros);
    void applyFilters();
    void clearFields();
    void loadFilters();
signals:
    void goWelcomeWidgetRequested();
private slots:
    void on_pushButtonDetalles_clicked();

private:
    Ui::DataTableWidget *ui;
    QSqlQueryModel *m_model = nullptr;
};

#endif // DATATABLEWIDGET_H
