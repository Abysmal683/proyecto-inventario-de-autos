#include "infodialog.h"
#include "ui_infodialog.h"
#include "imprimirdialog.h"
#include "databasemanager.h"
#include <QMessageBox>
#include <QSqlQuery>

InfoDialog::InfoDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::InfoDialog)
{
    ui->setupUi(this);
    if(parent)
        move(parent->geometry().center() - rect().center());
    // Llamar en el constructor o antes de cargar datos
    loadCombo(ui->comboBoxMarca, "vehiculos", "marca");
    loadCombo(ui->comboBoxModelo, "vehiculos", "modelo");
    loadCombo(ui->comboBoxEpoca, "vehiculos", "epoca");
    loadCombo(ui->comboBoxColor, "vehiculos", "color");
    loadCombo(ui->comboBoxPropietario, "vehiculos", "propietario");
    loadCombo(ui->comboBoxMotor, "datos_tecnicos", "motor");
    loadCombo(ui->comboBoxCarroceria, "datos_tecnicos", "carroceria");

}

InfoDialog::~InfoDialog()
{
    delete ui;
}
void InfoDialog::on_pushButtonRegresar_clicked(){
    close();
}
void InfoDialog::on_pushButtonImprimir_clicked(){
    ImprimirDialog imprimirDialog(this);
    imprimirDialog.setWindowTitle("Imprimir");
    imprimirDialog.exec();
}
// infodialog.cpp
void InfoDialog::setVehicleData(const QVariantMap &data)
{
    ui->lineEditVin->setText(data.value("vin").toString());
    ui->lineEditMatricula->setText(data.value("placa").toString());
    ui->comboBoxMarca->setCurrentText(data.value("marca").toString());
    ui->comboBoxModelo->setCurrentText(data.value("modelo").toString());
    ui->comboBoxEpoca->setCurrentText(QString::number(data.value("epoca").toInt()));
    ui->comboBoxColor->setCurrentText(data.value("color").toString());
    ui->comboBoxPropietario->setCurrentText(data.value("propietario").toString());

    ui->comboBoxMotor->setCurrentText(data.value("motor").toString());
    ui->spinBoxKilometraje->setValue(data.value("kilometraje").toInt());
    ui->spinBoxPuertas->setValue(data.value("puertas").toInt());
    ui->comboBoxCarroceria->setCurrentText(data.value("carroceria").toString());
    ui->lineEditDetalles->setText(data.value("detalles_adicionales").toString());

    ui->checkBoxEstado->setChecked(data.value("estado").toBool());

    // Cargar imagen si está disponible
    QByteArray imgData = data.value("foto").toByteArray();
    if(!imgData.isEmpty()) {
        QPixmap pix;
        pix.loadFromData(imgData);
        ui->labelImagen->setPixmap(pix.scaled(ui->labelImagen->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
}
void InfoDialog::on_pushButtonModificar_clicked()
{
    bool enable = ui->pushButtonModificar->isChecked();
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

    if(!enable) {
        // Guardar cambios cuando se desactive edición
        QVariantMap cambios;
        QString vin = ui->lineEditVin->text();
        cambios["placa"] = ui->lineEditMatricula->text();
        cambios["marca"] = ui->comboBoxMarca->currentText();
        cambios["modelo"] = ui->comboBoxModelo->currentText();
        cambios["epoca"] = ui->comboBoxEpoca->currentText().toInt();
        cambios["color"] = ui->comboBoxColor->currentText();
        cambios["propietario"] = ui->comboBoxPropietario->currentText();
        cambios["estado"] = ui->checkBoxEstado->isChecked();

        // Datos técnicos
        cambios["motor"] = ui->comboBoxMotor->currentText();
        cambios["kilometraje"] = ui->spinBoxKilometraje->value();
        cambios["puertas"] = ui->spinBoxPuertas->value();
        cambios["carroceria"] = ui->comboBoxCarroceria->currentText();
        cambios["detalles_adicionales"] = ui->lineEditDetalles->text();

        // Actualizar BD
        if(!DatabaseManager::instance().updateVehicle(vin, cambios) ||
            !DatabaseManager::instance().updateTechnicalData(
                DatabaseManager::instance().getTechnicalData(vin)["id"].toInt(), cambios)) {
            QMessageBox::critical(this, "Error", "No se pudieron guardar los cambios.");
        } else {
            emit vehicleUpdated(vin);
            QMessageBox::information(this, "Éxito", "Registro actualizado correctamente.");
        }
    }
}
void InfoDialog::on_pushButtonEliminarRegistro_clicked()
{
    QString vin = ui->lineEditVin->text();
    if(QMessageBox::question(this, "Confirmar", "¿Eliminar este vehículo?") == QMessageBox::Yes) {
        bool ok1 = DatabaseManager::instance().deleteVehicle(vin);
        bool ok2 = DatabaseManager::instance().deleteTechnicalData(
            DatabaseManager::instance().getTechnicalData(vin)["id"].toInt());
        if(ok1 && ok2) {
            emit vehicleDeleted(vin);
            close();
        } else {
            QMessageBox::critical(this, "Error", "No se pudo eliminar el vehículo.");
        }
    }
}
void InfoDialog::clearFields()
{
    // Limpiar todos los campos
    ui->lineEditVin->clear();
    ui->lineEditMatricula->clear();
    ui->comboBoxMarca->setCurrentIndex(0);
    ui->comboBoxModelo->setCurrentIndex(0);
    ui->comboBoxEpoca->setCurrentIndex(0);
    ui->comboBoxColor->setCurrentIndex(0);
    ui->comboBoxPropietario->setCurrentIndex(0);
    ui->comboBoxMotor->setCurrentIndex(0);
    ui->comboBoxCarroceria->setCurrentIndex(0);
    ui->lineEditDetalles->clear();
    ui->checkBoxEstado->setChecked(false);

    ui->labelImagen->setText("Sin imagen");
    ui->pushButtonModificar->setChecked(false);

    // Deshabilitar edición
    ui->lineEditVin->setEnabled(false);
    ui->lineEditMatricula->setEnabled(false);
    ui->comboBoxMarca->setEnabled(false);
    ui->comboBoxModelo->setEnabled(false);
    ui->comboBoxEpoca->setEnabled(false);
    ui->comboBoxColor->setEnabled(false);
    ui->comboBoxPropietario->setEnabled(false);
    ui->comboBoxMotor->setEnabled(false);
    ui->comboBoxCarroceria->setEnabled(false);
    ui->lineEditDetalles->setEnabled(false);
    ui->checkBoxEstado->setEnabled(false);
    ui->pushButtonSeleccionarImagen->setEnabled(false);
}
void InfoDialog::loadCombo(QComboBox* combo, const QString &table, const QString &col)
{
    combo->blockSignals(true);
    combo->clear();
    combo->addItem("");

    QSqlQuery q(DatabaseManager::instance().getDatabase());
    QString str = QString("SELECT DISTINCT %1 FROM %2 "
                          "WHERE %1 IS NOT NULL AND %1 != '' ORDER BY %1")
                      .arg(col, table);

    if (q.exec()) {
        while (q.next()) {
            combo->addItem(q.value(0).toString());
        }
    }

    combo->blockSignals(false);
}
