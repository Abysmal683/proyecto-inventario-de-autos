// src/infodialog.cpp (fragmentos relevantes)
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

InfoDialog::InfoDialog(QWidget *parent)
    : QDialog(parent),
    ui(new Ui::InfoDialog)
{
    ui->setupUi(this);

    if (parent)
        move(parent->geometry().center() - rect().center());

    // Cargar combos (dejamos esto, los combos están bien)
    loadCombo(ui->comboBoxMarca, "vehiculos", "marca");
    loadCombo(ui->comboBoxModelo, "vehiculos", "modelo");
    loadCombo(ui->comboBoxEpoca, "vehiculos", "epoca");
    loadCombo(ui->comboBoxColor, "vehiculos", "color");
    loadCombo(ui->comboBoxPropietario, "vehiculos", "propietario");

    loadCombo(ui->comboBoxMotor, "datos_tecnicos", "motor");
    loadCombo(ui->comboBoxCarroceria, "datos_tecnicos", "carroceria");

    // -- IMPORTANTE: NO conectar aquí el clicked a on_pushButtonSeleccionarImagen_clicked
    // Si la función se llama on_pushButtonSeleccionarImagen_clicked(), Qt la llama automáticamente.
    // Evita duplicados.

    enableEditing(false);
}

InfoDialog::~InfoDialog()
{
    delete ui;
}
// --------------------- Cargar datos del vehículo ---------------------
void InfoDialog::setVehicleData(const QString &vin)
{
    m_vin = vin; // Guardamos el VIN como identificador único

    // Obtener datos de la base de datos
    QVariantMap vehiculo = DatabaseManager::instance().getVehicle(vin);
    QVariantMap tecnico  = DatabaseManager::instance().getTechnicalData(vin);

    if (vehiculo.isEmpty()) {
        QMessageBox::warning(this, "Aviso", "No se encontró el vehículo con VIN: " + vin);
        return;
    }

    // Datos del vehículo
    ui->lineEditVin->setText(vehiculo.value("vin").toString());
    ui->lineEditMatricula->setText(vehiculo.value("placa").toString());
    ui->comboBoxMarca->setCurrentText(vehiculo.value("marca").toString());
    ui->comboBoxModelo->setCurrentText(vehiculo.value("modelo").toString());
    ui->comboBoxEpoca->setCurrentText(QString::number(vehiculo.value("epoca").toInt()));
    ui->comboBoxColor->setCurrentText(vehiculo.value("color").toString());
    ui->comboBoxPropietario->setCurrentText(vehiculo.value("propietario").toString());
    ui->checkBoxEstado->setChecked(vehiculo.value("estado").toBool());

    // Datos técnicos
    ui->comboBoxMotor->setCurrentText(tecnico.value("motor").toString());
    ui->spinBoxKilometraje->setValue(tecnico.value("kilometraje").toInt());
    ui->spinBoxPuertas->setValue(tecnico.value("puertas").toInt());
    ui->comboBoxCarroceria->setCurrentText(tecnico.value("carroceria").toString());
    ui->lineEditDetalles->setText(tecnico.value("detalles_adicionales").toString());

    // Imagen
    QByteArray img = vehiculo.value("foto").toByteArray();
    setVehicleImage(img);
}
void InfoDialog::setVehicleImage(const QByteArray &img)
{
    if (!img.isEmpty()) {
        currentImage = img; // Guardar la imagen
        QPixmap pix;
        pix.loadFromData(img);
        ui->labelImagen->setPixmap(pix.scaled(ui->labelImagen->size(),
                                              Qt::KeepAspectRatio,
                                              Qt::SmoothTransformation));
    } else {
        currentImage.clear();
        ui->labelImagen->setText("Sin imagen");
        ui->labelImagen->setPixmap(QPixmap());
    }
}
// --------------------- Seleccionar Imagen ---------------------
void InfoDialog::on_pushButtonSeleccionarImagen_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(
        this,
        "Seleccionar Imagen",
        "",
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

// --------------------- Habilitar edición ---------------------
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

// --------------------- Guardar cambios ---------------------
bool InfoDialog::saveChanges()
{
    // usa el VIN guardado al abrir
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

// --------------------- Botón Modificar ---------------------
void InfoDialog::on_pushButtonModificar_clicked()
{
    bool enable = ui->pushButtonModificar->isChecked();
    enableEditing(enable);

    if (!enable) { // Guardar cambios
        if (!saveChanges()) {
            QMessageBox::critical(this, "Error", "No se pudieron guardar los cambios.");
        } else {
            QMessageBox::information(this, "Éxito", "Registro actualizado.");
        }
    }
}

// --------------------- Botón Eliminar ---------------------
void InfoDialog::on_pushButtonEliminarRegistro_clicked()
{
    QString vin = m_vin;
    if (vin.isEmpty()) {
        QMessageBox::warning(this, "Aviso", "No hay un VIN válido para eliminar.");
        return;
    }

    QMessageBox::StandardButton reply =
        QMessageBox::question(this,
                              "Confirmar eliminación",
                              "¿Está seguro de eliminar este registro?",
                              QMessageBox::Yes | QMessageBox::No);

    if (reply != QMessageBox::Yes) return;

    bool ok1 = DatabaseManager::instance().deleteVehicle(vin);
    int idTec = DatabaseManager::instance().getTechnicalData(vin)["id"].toInt();
    bool ok2 = DatabaseManager::instance().deleteTechnicalData(idTec);

    if (!ok1 || !ok2) {
        QMessageBox::critical(this, "Error", "No se pudo eliminar el registro.");
        return;
    }

    emit vehicleDeleted(vin);
    QMessageBox::information(this, "Listo", "Registro eliminado correctamente.");
    accept();
}

// --------------------- Botón Regresar ---------------------
void InfoDialog::on_pushButtonRegresar_clicked()
{
    reject();
}

// --------------------- Botón Imprimir ---------------------
void InfoDialog::on_pushButtonImprimir_clicked()
{
    ImprimirDialog imprimirDialog(this);
    imprimirDialog.exec();
}

// --------------------- Función para llenar combos ---------------------
void InfoDialog::loadCombo(QComboBox* combo, const QString &table, const QString &col)
{
    if (!combo) return;

    combo->clear();
    combo->addItem("");

    QSqlQuery query(DatabaseManager::instance().getDatabase());
    query.prepare(QString("SELECT DISTINCT %1 FROM %2 WHERE %1 IS NOT NULL AND %1 != '' ORDER BY %1")
                      .arg(col, table));
    if (query.exec()) {
        while (query.next()) {
            combo->addItem(query.value(0).toString());
        }
    } else {
        qDebug() << "Error al cargar combo" << col << ":" << query.lastError();
    }
}
