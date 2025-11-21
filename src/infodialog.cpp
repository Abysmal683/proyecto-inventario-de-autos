#include "infodialog.h"
#include "ui_infodialog.h"
#include "imprimirdialog.h"
#include "databasemanager.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QBuffer>
#include <QSqlQuery>
#include <QSqlError>
#include <QComboBox>
#include <QPixmap>
#include <QDebug>
#include <QImage>

InfoDialog::InfoDialog(QWidget *parent)
    : QDialog(parent),
    ui(new Ui::InfoDialog)
{
    ui->setupUi(this);

    if (parent)
        move(parent->geometry().center() - rect().center());

    loadCombo(ui->comboBoxMarca, "vehiculos", "marca");
    loadCombo(ui->comboBoxModelo, "vehiculos", "modelo");
    loadCombo(ui->comboBoxEpoca, "vehiculos", "epoca");
    loadCombo(ui->comboBoxColor, "vehiculos", "color");
    loadCombo(ui->comboBoxPropietario, "vehiculos", "propietario");

    loadCombo(ui->comboBoxMotor, "datos_tecnicos", "motor");
    loadCombo(ui->comboBoxCarroceria, "datos_tecnicos", "carroceria");

    enableEditing(false);
}

InfoDialog::~InfoDialog()
{
    delete ui;
}

void InfoDialog::setVehicleData(const QString &vin)
{
    m_vin = vin;

    QVariantMap vehiculo = DatabaseManager::instance().getVehicle(vin);
    QVariantMap tecnico  = DatabaseManager::instance().getTechnicalData(vin);

    if (vehiculo.isEmpty()) {
        QMessageBox::warning(this, "Aviso", "No se encontró el vehículo con VIN: " + vin);
        return;
    }

    ui->lineEditVin->setText(vehiculo["vin"].toString());
    ui->lineEditMatricula->setText(vehiculo["placa"].toString());
    ui->comboBoxMarca->setCurrentText(vehiculo["marca"].toString());
    ui->comboBoxModelo->setCurrentText(vehiculo["modelo"].toString());
    ui->comboBoxEpoca->setCurrentText(QString::number(vehiculo["epoca"].toInt()));
    ui->comboBoxColor->setCurrentText(vehiculo["color"].toString());
    ui->comboBoxPropietario->setCurrentText(vehiculo["propietario"].toString());
    ui->checkBoxEstado->setChecked(vehiculo["estado"].toBool());

    ui->comboBoxMotor->setCurrentText(tecnico["motor"].toString());
    ui->spinBoxKilometraje->setValue(tecnico["kilometraje"].toInt());
    ui->spinBoxPuertas->setValue(tecnico["puertas"].toInt());
    ui->comboBoxCarroceria->setCurrentText(tecnico["carroceria"].toString());
    ui->lineEditDetalles->setText(tecnico["detalles_adicionales"].toString());

    setVehicleImage(vehiculo["foto"].toByteArray());
}

void InfoDialog::setVehicleImage(const QByteArray &img)
{
    if (!img.isEmpty()) {
        currentImage = img;

        QPixmap pix;
        pix.loadFromData(img);

        ui->labelImagen->setPixmap(
            pix.scaled(ui->labelImagen->size(),
                       Qt::KeepAspectRatio,
                       Qt::SmoothTransformation)
            );
    } else {
        currentImage.clear();
        ui->labelImagen->setText("Sin imagen");
        ui->labelImagen->setPixmap(QPixmap());
    }
}

// =============================================
// GENERAR DATOS PARA PDF
// =============================================
VehiclePrintData InfoDialog::getPrintData() const
{
    VehiclePrintData d;

    d.vin = ui->lineEditVin->text();
    d.placa = ui->lineEditMatricula->text();
    d.marca = ui->comboBoxMarca->currentText();
    d.modelo = ui->comboBoxModelo->currentText();
    d.epoca = ui->comboBoxEpoca->currentText();
    d.color = ui->comboBoxColor->currentText();
    d.propietario = ui->comboBoxPropietario->currentText();

    d.motor = ui->comboBoxMotor->currentText();
    d.kilometraje = QString::number(ui->spinBoxKilometraje->value());
    d.puertas = QString::number(ui->spinBoxPuertas->value());
    d.carroceria = ui->comboBoxCarroceria->currentText();
    d.detalles = ui->lineEditDetalles->text();

    d.estado = ui->checkBoxEstado->isChecked() ? "Activo" : "Inactivo";

    if (!currentImage.isEmpty()) {
        QPixmap pix;
        pix.loadFromData(currentImage);
        d.imagen = pix.toImage();
    }

    return d;
}

void InfoDialog::on_pushButtonSeleccionarImagen_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(
        this, "Seleccionar Imagen", "",
        "Imagenes (*.png *.jpg *.bmp)");

    if (fileName.isEmpty()) return;

    QPixmap pix(fileName);
    if (pix.isNull()) return;

    QByteArray bufferData;
    QBuffer buffer(&bufferData);
    buffer.open(QIODevice::WriteOnly);
    pix.save(&buffer, "PNG");

    setVehicleImage(bufferData);
}

void InfoDialog::enableEditing(bool enable)
{
    ui->comboBoxMarca->setEnabled(enable);
    ui->comboBoxModelo->setEnabled(enable);
    ui->comboBoxEpoca->setEnabled(enable);
    ui->comboBoxColor->setEnabled(enable);
    ui->lineEditMatricula->setEnabled(enable);
    ui->comboBoxPropietario->setEnabled(enable);

    ui->comboBoxMotor->setEnabled(enable);
    ui->spinBoxKilometraje->setEnabled(enable);
    ui->spinBoxPuertas->setEnabled(enable);
    ui->comboBoxCarroceria->setEnabled(enable);
    ui->lineEditDetalles->setEnabled(enable);
    ui->checkBoxEstado->setEnabled(enable);

    ui->pushButtonSeleccionarImagen->setEnabled(enable);
}

bool InfoDialog::saveChanges()
{
    QString vin = m_vin;
    if (vin.isEmpty()) return false;

    QVariantMap vehiculo;
    vehiculo["placa"] = ui->lineEditMatricula->text();
    vehiculo["marca"] = ui->comboBoxMarca->currentText();
    vehiculo["modelo"] = ui->comboBoxModelo->currentText();
    vehiculo["epoca"] = ui->comboBoxEpoca->currentText().toInt();
    vehiculo["color"] = ui->comboBoxColor->currentText();
    vehiculo["propietario"] = ui->comboBoxPropietario->currentText();
    vehiculo["estado"] = ui->checkBoxEstado->isChecked();
    vehiculo["foto"] = currentImage;

    QVariantMap tecnico;
    tecnico["motor"] = ui->comboBoxMotor->currentText();
    tecnico["kilometraje"] = ui->spinBoxKilometraje->value();
    tecnico["puertas"] = ui->spinBoxPuertas->value();
    tecnico["carroceria"] = ui->comboBoxCarroceria->currentText();
    tecnico["detalles_adicionales"] = ui->lineEditDetalles->text();

    int idTec = DatabaseManager::instance().getTechnicalData(vin)["id"].toInt();

    bool ok1 = DatabaseManager::instance().updateVehicle(vin, vehiculo);
    bool ok2 = DatabaseManager::instance().updateTechnicalData(idTec, tecnico);

    if (!ok1 || !ok2) return false;

    emit vehicleUpdated(vin);
    return true;
}

void InfoDialog::on_pushButtonModificar_clicked()
{
    bool enable = ui->pushButtonModificar->isChecked();
    enableEditing(enable);

    if (!enable) {
        if (!saveChanges()) {
            QMessageBox::critical(this, "Error", "No se pudieron guardar los cambios.");
        } else {
            QMessageBox::information(this, "Éxito", "Registro actualizado.");
        }
    }
}

void InfoDialog::on_pushButtonEliminarRegistro_clicked()
{
    QString vin = m_vin;
    if (vin.isEmpty()) {
        QMessageBox::warning(this, "Aviso", "No hay un VIN válido para eliminar.");
        return;
    }

    if (QMessageBox::question(this, "Confirmar eliminación",
                              "¿Está seguro de eliminar este registro?",
                              QMessageBox::Yes | QMessageBox::No)
        != QMessageBox::Yes)
        return;

    bool ok1 = DatabaseManager::instance().deleteVehicle(vin);
    int idTec = DatabaseManager::instance().getTechnicalData(vin)["id"].toInt();
    bool ok2 = DatabaseManager::instance().deleteTechnicalData(idTec);

    if (!ok1 || !ok2) {
        QMessageBox::critical(this, "Error", "No se pudo eliminar el registro.");
        return;
    }

    emit vehicleDeleted(vin);
    QMessageBox::information(this, "Listo", "Registro eliminado.");
    accept();
}

void InfoDialog::on_pushButtonRegresar_clicked()
{
    reject();
}

void InfoDialog::on_pushButtonImprimir_clicked()
{
    VehiclePrintData data = getPrintData();

    ImprimirDialog dlg(this);
    dlg.setContent(data);
    dlg.exec();
}
// agrega esto en src/infodialog.cpp

// ... (otros includes ya presentes)

void InfoDialog::loadCombo(QComboBox* combo, const QString &table, const QString &col)
{
    if (!combo) return;

    QString old = combo->currentText();
    combo->blockSignals(true);
    combo->clear();
    combo->addItem("");

    QSqlQuery q(DatabaseManager::instance().getDatabase());
    QString str = QString("SELECT DISTINCT %1 FROM %2 WHERE %1 IS NOT NULL AND %1 != '' ORDER BY %1")
                      .arg(col, table);
    if (q.exec(str)) {
        while (q.next()) {
            combo->addItem(q.value(0).toString());
        }
    } else {
        qDebug() << "InfoDialog::loadCombo ERROR:" << q.lastError() << " QUERY:" << str;
    }

    int idx = combo->findText(old);
    combo->setCurrentIndex(idx >= 0 ? idx : 0);
    combo->blockSignals(false);
}
