#ifndef INFODIALOG_H
#define INFODIALOG_H

#include <QDialog>
#include <QComboBox>
namespace Ui {
class InfoDialog;
}

class InfoDialog : public QDialog
{
    Q_OBJECT

public:
    explicit InfoDialog(QWidget *parent = nullptr);
    ~InfoDialog();
    void setVehicleData(const QVariantMap &data);
    void clearFields();

private slots:
    void on_pushButtonRegresar_clicked();
    void on_pushButtonImprimir_clicked();
    void on_pushButtonModificar_clicked();
    void on_pushButtonEliminarRegistro_clicked();

signals:
    void vehicleUpdated(const QString &vin);
    void vehicleDeleted(const QString &vin);

private:
    Ui::InfoDialog *ui;
    void loadCombo(QComboBox* combo, const QString &table, const QString &col);
};

#endif // INFODIALOG_H
