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
    QSqlQuery query(DatabaseManager::instance().getDatabase());

    auto fillCombo = [&](QComboBox* combo, const QString& column){
        query.exec(QString("SELECT DISTINCT %1 FROM vehiculos ORDER BY %1").arg(column));
        combo->clear();
        while(query.next()) {
            QString value = query.value(0).toString();
            if (!value.isEmpty())
                combo->addItem(value);
        }
    };

    fillCombo(ui->comboBoxMarca, "marca");
    fillCombo(ui->comboBoxModelo, "modelo");
    fillCombo(ui->comboBoxEpoca, "epoca");
    fillCombo(ui->comboBoxColor, "color");
    fillCombo(ui->comboBoxMotor, "motor");
    fillCombo(ui->comboBoxCarroceria, "carroceria");
    fillCombo(ui->comboBoxPropietario, "propietario");
}
