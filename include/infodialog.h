#ifndef INFODIALOG_H
#define INFODIALOG_H

#include <QDialog>
#include <QComboBox>
#include <QImage>
#include <QByteArray>

struct VehiclePrintData {
    QString vin;
    QString placa;
    QString marca;
    QString modelo;
    QString epoca;
    QString color;
    QString propietario;
    QString motor;
    QString kilometraje;
    QString puertas;
    QString carroceria;
    QString detalles;
    QString estado;
    QImage imagen;
};

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
    VehiclePrintData getPrintData() const;

signals:
    void vehicleUpdated(const QString &vin);
    void vehicleDeleted(const QString &vin);

private slots:
    void on_pushButtonRegresar_clicked();
    void on_pushButtonImprimir_clicked();
    void on_pushButtonModificar_clicked();
    void on_pushButtonEliminarRegistro_clicked();
    void on_pushButtonSeleccionarImagen_clicked();

private:
    Ui::InfoDialog *ui;

    QByteArray currentImage;
    QString m_vin;

    void enableEditing(bool enable);
    bool saveChanges();
    void setVehicleImage(const QByteArray &img);
    void loadCombo(QComboBox *combo, const QString &table, const QString &col);
};

#endif // INFODIALOG_H
