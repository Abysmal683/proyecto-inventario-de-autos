#ifndef INFODIALOG_H
#define INFODIALOG_H

#include <QDialog>
#include <QComboBox>
#include <QByteArray>

namespace Ui {
class InfoDialog;
}

class InfoDialog : public QDialog
{
    Q_OBJECT

public:
    explicit InfoDialog(QWidget *parent = nullptr);
    ~InfoDialog();

    void setVehicleData(const QString &vin);
    void loadCombo(QComboBox* combo, const QString &table, const QString &col);

signals:
    void vehicleUpdated(const QString &vin);
    void vehicleDeleted(const QString &vin);

private slots:
    void on_pushButtonRegresar_clicked();
    void on_pushButtonImprimir_clicked();
    void on_pushButtonModificar_clicked();
    void on_pushButtonEliminarRegistro_clicked();
    void on_pushButtonSeleccionarImagen_clicked();     // NUEVO SLOT

private:
    Ui::InfoDialog *ui;

    QByteArray currentImage;  // NUEVO → imagen actual

    QString m_vin;
    void enableEditing(bool enable);           // NUEVO
    bool saveChanges();                        // NUEVO
    void setVehicleImage(const QByteArray& img); // NUEVO
};

#endif // INFODIALOG_H
