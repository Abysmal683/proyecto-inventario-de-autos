#include "newregistrationwidget.h"
#include "ui_newregistrationwidget.h"
#include "databasemanager.h"
#include <QFileDialog>
#include <QBuffer>
#include <QPixmap>
#include <QMessageBox>
#include <QStringListModel>
#include <QSqlQuery>
#include <QSqlDatabase>
NewRegistrationWidget::NewRegistrationWidget(QWidget *parent)
    : QWidget(parent), ui(new Ui::NewRegistrationWidget)
{
    ui->setupUi(this);
    clearFields();
    loadReferenceData();
    connect(ui->pushButtonCancelar, &QPushButton::clicked, this, [=](){
        emit goWelcomeWidgetRequested();
    });

    connect(ui->pushButtonConfirmar, &QPushButton::clicked,
            this, &NewRegistrationWidget::onPushButtonConfirmarClicked);

    connect(ui->pushButtonSeleccionarImagen, &QPushButton::clicked,
            this, &NewRegistrationWidget::onPushButtonSeleccionarImagenClicked);

    setupCompleters();
}

NewRegistrationWidget::~NewRegistrationWidget()
{
    delete ui;
}

// Selección de imagen
void NewRegistrationWidget::onPushButtonSeleccionarImagenClicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Seleccionar Imagen"), "",
        tr("Imagenes (*.png *.jpg *.bmp)"));

    if(fileName.isEmpty()) return;

    QPixmap pix(fileName);
    ui->labelImagen->setPixmap(pix.scaled(ui->labelImagen->size(),
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation));

    QBuffer buffer(&vehicleImage);
    buffer.open(QIODevice::WriteOnly);
    pix.save(&buffer, "PNG");
}

// Confirmar registro
void NewRegistrationWidget::onPushButtonConfirmarClicked()
{
    QString vin = ui->lineEditVin->text().trimmed();
    QString marca = ui->comboBoxMarca->currentText().trimmed();
    QString modelo = ui->comboBoxModelo->currentText().trimmed();
    int epoca = ui->comboBoxEpoca->currentText().toInt();
    QString color = ui->comboBoxColor->currentText().trimmed();
    QString placa = ui->lineEditMatricula->text().trimmed();
    QString propietario = ui->comboBoxPropietario->currentText().trimmed();
    bool estado = true; // Por defecto

    if(vin.isEmpty() || placa.isEmpty()) {
        QMessageBox::warning(this, tr("Campos incompletos"),
            tr("VIN y Placa son obligatorios."));
        return;
    }

    bool successVehicle = DatabaseManager::instance().addVehicle(
        vin, marca, modelo, epoca, color, placa, propietario, vehicleImage, estado
        );

    if(!successVehicle) {
        QMessageBox::critical(this, tr("Error"),
            tr("No se pudo agregar el vehículo."));
        return;
    }

    // Datos técnicos
    QString motor = ui->comboBoxMotor->currentText().trimmed();
    int kilometraje = ui->spinBoxKilometraje->value();
    int puertas = ui->spinBoxPuertas->value();
    QString carroceria = ui->comboBoxCarroceria->currentText().trimmed();
    QString detalles = ui->lineEditDetalles->text().trimmed();

    bool successTechnical = DatabaseManager::instance().addTechnicalData(
        vin, motor, kilometraje, puertas, carroceria, detalles
        );

    if(!successTechnical) {
        QMessageBox::critical(this, tr("Error"),
           tr("No se pudo agregar los datos técnicos."));
        return;
    }

    QVariantMap newVehicle;
    newVehicle["vin"] = vin;
    newVehicle["marca"] = marca;
    newVehicle["modelo"] = modelo;
    newVehicle["epoca"] = epoca;
    newVehicle["color"] = color;
    newVehicle["placa"] = placa;
    newVehicle["propietario"] = propietario;

    emit vehicleAdded(newVehicle);

    QMessageBox::information(this, tr("Éxito"),
        tr("Vehículo agregado correctamente."));
    emit goWelcomeWidgetRequested();
}

// Configurar QCompleter para VIN y Placa
void NewRegistrationWidget::setupCompleters()
{
    vinCompleter = new QCompleter(this);
    ui->lineEditVin->setCompleter(vinCompleter);

    placaCompleter = new QCompleter(this);
    ui->lineEditMatricula->setCompleter(placaCompleter);

    loadCompleterData();
}

// Cargar datos existentes desde la base
void NewRegistrationWidget::loadCompleterData()
{
    QStringList vinList = DatabaseManager::instance().getAllVINs();
    vinCompleter->setModel(new QStringListModel(vinList, vinCompleter));

    QStringList placaList = DatabaseManager::instance().getAllPlacas();
    placaCompleter->setModel(new QStringListModel(placaList, placaCompleter));
}
void NewRegistrationWidget::loadReferenceData()
{
    QSqlDatabase db = DatabaseManager::instance().getDatabase();
    QSqlQuery query(db);

    // Helper para cargar combos desde consultas
    auto fillComboQuery = [&](QComboBox* combo, const QString& sql)
    {
        QString old = combo->currentText();
        combo->blockSignals(true);
        combo->clear();
        combo->addItem("");

        query.exec(sql);
        while (query.next()) {
            QString v = query.value(0).toString();
            if (!v.isEmpty())
                combo->addItem(v);
        }

        int idx = combo->findText(old);
        combo->setCurrentIndex(idx >= 0 ? idx : 0);
        combo->blockSignals(false);
    };

    // --- Datos de vehiculos ---
    fillComboQuery(ui->comboBoxMarca,
                   "SELECT DISTINCT marca FROM vehiculos "
                   "WHERE marca != '' ORDER BY marca");

    fillComboQuery(ui->comboBoxModelo,
                   "SELECT DISTINCT modelo FROM vehiculos "
                   "WHERE modelo != '' ORDER BY modelo");

    fillComboQuery(ui->comboBoxEpoca,
                   "SELECT DISTINCT epoca FROM vehiculos "
                   "WHERE epoca IS NOT NULL AND epoca != '' ORDER BY epoca");

    fillComboQuery(ui->comboBoxColor,
                   "SELECT DISTINCT color FROM vehiculos "
                   "WHERE color != '' ORDER BY color");

    fillComboQuery(ui->comboBoxPropietario,
                   "SELECT DISTINCT propietario FROM vehiculos "
                   "WHERE propietario != '' ORDER BY propietario");

    // --- Datos técnicos ---
    fillComboQuery(ui->comboBoxMotor,
                   "SELECT DISTINCT motor FROM datos_tecnicos "
                   "WHERE motor != '' ORDER BY motor");

    fillComboQuery(ui->comboBoxCarroceria,
                   "SELECT DISTINCT carroceria FROM datos_tecnicos "
                   "WHERE carroceria != '' ORDER BY carroceria");
}
void NewRegistrationWidget::clearFields(){
    // LineEdits
    ui->lineEditVin->clear();
    ui->lineEditMatricula->clear();
    ui->lineEditDetalles->clear();

    // ComboBoxes -> ponemos en -1 o agregamos opción vacía y seleccionamos índice 0
    auto clearCombo = [&](QComboBox *c){
        // Si tiene item vacío en la posición 0, seleccionamos 0; si no, deseleccionamos
        if (c->findText("") == -1) {
            c->insertItem(0, "");
        }
        c->setCurrentIndex(0);
    };

    clearCombo(ui->comboBoxMarca);
    clearCombo(ui->comboBoxModelo);
    clearCombo(ui->comboBoxEpoca);
    clearCombo(ui->comboBoxColor);
    clearCombo(ui->comboBoxMotor);
    clearCombo(ui->comboBoxCarroceria);
    clearCombo(ui->comboBoxPropietario);

    // Spinners
    ui->spinBoxKilometraje->setValue(0);
    ui->spinBoxPuertas->setValue(0);

    // Imagen y buffer
    vehicleImage.clear();
    QPixmap defaultImg(":/images/buho.jpg");
    ui->labelImagen->setPixmap(defaultImg);

}
