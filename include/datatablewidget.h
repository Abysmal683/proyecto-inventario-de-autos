#ifndef DATATABLEWIDGET_H
#define DATATABLEWIDGET_H

#include <QWidget>
#include <QSqlQueryModel>
#include <QItemSelectionModel>
#include <QVariantMap>

class InfoDialog;   // NUEVO → Adelanto porque lo usamos como puntero

namespace Ui {
class DataTableWidget;
}

class DataTableWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DataTableWidget(QWidget *parent = nullptr);
    ~DataTableWidget();

    // ---------- Filtros ----------
    void setFilters(const QVariantMap &filtros);
    void applyFilters();
    void clearFields();
    void loadFilters();

    // ---------- InfoDialog ----------
    void loadInfoDialog();      // NUEVO → Inicializa InfoDialog y carga datos
    void clearInfoDialog();     // NUEVO → Limpia InfoDialog si es necesario

signals:
    void goWelcomeWidgetRequested();

private slots:
    void on_pushButtonDetalles_clicked();     // Abre el dialog con detalles
    void onRowSelected(const QModelIndex &current, const QModelIndex &); // Actualiza foto y datos

private:
    Ui::DataTableWidget *ui;

    QSqlQueryModel *m_model = nullptr;

    QVariantMap m_currentFilters;     // NUEVO → guarda filtros activos

    InfoDialog *m_infoDialog = nullptr;   // NUEVO → ventana de detalles
    void abrirInfoDialog();
};

#endif // DATATABLEWIDGET_H
