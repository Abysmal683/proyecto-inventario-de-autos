#include "welcomewidget.h"
#include "ui_welcomewidget.h"
#include "imprimirdialog.h"
#include "databasemanager.h"
#include <QSqlQuery>
#include <QSqlDatabase>
WelcomeWidget::WelcomeWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::WelcomeWidget)
{
    ui->setupUi(this);
    loadFilters();
    connect(ui->pushButtonRegistros, &QPushButton::clicked, this, [=](){
        emit goDataTableWidgetRequested();
    });
    connect(ui->pushButtonFinalizar,&QPushButton::clicked,this,[=](){
       emit finishMainWindowRequested();
    });
    connect(ui->pushButtonNuevoRegistro,&QPushButton::clicked,this,[=](){
       emit goNewRegistrationWidgetRequested();
    });

    connect(ui->pushButtonBuscar, &QPushButton::clicked, this,&WelcomeWidget::onPushButtonBuscarClicked);
}
WelcomeWidget::~WelcomeWidget()
{
    delete ui;
}
void WelcomeWidget::loadFilters()
{
    QSqlQuery query(DatabaseManager::instance().getDatabase());

    auto fillCombo = [&](QComboBox* combo, const QString& column){
        query.exec(QString("SELECT DISTINCT %1 FROM vehiculos ORDER BY %1").arg(column));
        combo->clear();
        combo->addItem(""); // opción vacía = sin filtro
        while(query.next())
            combo->addItem(query.value(0).toString());
    };

    fillCombo(ui->comboBoxMarca, "marca");
    fillCombo(ui->comboBoxModelo, "modelo");
    fillCombo(ui->comboBoxEpoca, "epoca");
    fillCombo(ui->comboBoxColor, "color");
    fillCombo(ui->comboBoxMotor, "motor");
    fillCombo(ui->comboBoxCarroceria, "carroceria");
    fillCombo(ui->comboBoxPropietario, "propietario");
}
void WelcomeWidget::onPushButtonBuscarClicked(){
    QVariantMap filtros;

    if(!ui->comboBoxMarca->currentText().isEmpty())
        filtros["marca"] = ui->comboBoxMarca->currentText();
    if(!ui->comboBoxModelo->currentText().isEmpty())
        filtros["modelo"] = ui->comboBoxModelo->currentText();
    if(!ui->comboBoxEpoca->currentText().isEmpty())
        filtros["epoca"] = ui->comboBoxEpoca->currentText().toInt();
    if(!ui->comboBoxColor->currentText().isEmpty())
        filtros["color"] = ui->comboBoxColor->currentText();
    if(!ui->comboBoxMotor->currentText().isEmpty())
        filtros["motor"] = ui->comboBoxMotor->currentText();
    if(!ui->comboBoxCarroceria->currentText().isEmpty())
        filtros["carroceria"] = ui->comboBoxCarroceria->currentText();
    if(!ui->comboBoxPropietario->currentText().isEmpty())
        filtros["propietario"] = ui->comboBoxPropietario->currentText();
    if(!ui->lineEditVin->text().isEmpty())
        filtros["vin"] = ui->lineEditVin->text();
    if(!ui->lineEditMatricula->text().isEmpty())
        filtros["placa"] = ui->lineEditMatricula->text();
    filtros["estado"] = ui->checkBoxEstado->isChecked(); // bool
    // Emitimos signal con filtros
    emit goDataTableWidgetAndResearchRequested(filtros);
}
