#include "welcomewidget.h"
#include "ui_welcomewidget.h"
#include "imprimirdialog.h"
#include "databasemanager.h"
#include <QSqlQuery>
#include <QSqlDatabase>
#include <QVariant>
WelcomeWidget::WelcomeWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::WelcomeWidget)
{
    ui->setupUi(this);
    loadFilters();
    connect(ui->pushButtonRegistros, &QPushButton::clicked, this, [=](){
        emit goDataTableWidgetRequested();
    });

    connect(ui->pushButtonFinalizar, &QPushButton::clicked, this, [=](){
        emit finishMainWindowRequested();
    });

    connect(ui->pushButtonNuevoRegistro, &QPushButton::clicked, this, [=](){
        emit goNewRegistrationWidgetRequested();
    });

    connect(ui->pushButtonBuscar, &QPushButton::clicked,
            this, &WelcomeWidget::onPushButtonBuscarClicked);
}

WelcomeWidget::~WelcomeWidget()
{
    delete ui;
}

void WelcomeWidget::loadFilters()
{
    QSqlQuery query(DatabaseManager::instance().getDatabase());

    auto fillCombo = [&](QComboBox *combo, const QString &table, const QString &column)
    {
        combo->blockSignals(true);
        combo->clear();
        combo->addItem("");  // siempre primera opción vacía

        const QString sql = QStringLiteral(
                                "SELECT DISTINCT %1 FROM %2 "
                                "WHERE %1 IS NOT NULL AND %1 != '' "
                                "ORDER BY %1"
                                ).arg(column, table);

        if (query.exec(sql)) {
            while (query.next()) {
                const QString value = query.value(0).toString();
                if (!value.isEmpty())
                    combo->addItem(value);
            }
        }

        combo->setCurrentIndex(0);
        combo->blockSignals(false);
    };

    // Vehículos
    fillCombo(ui->comboBoxMarca,       "vehiculos",      "marca");
    fillCombo(ui->comboBoxModelo,      "vehiculos",      "modelo");
    fillCombo(ui->comboBoxEpoca,       "vehiculos",      "epoca");
    fillCombo(ui->comboBoxColor,       "vehiculos",      "color");
    fillCombo(ui->comboBoxPropietario, "vehiculos",      "propietario");

    // Datos técnicos (tabla datos_tecnicos)
    fillCombo(ui->comboBoxMotor,       "datos_tecnicos", "motor");
    fillCombo(ui->comboBoxCarroceria,  "datos_tecnicos", "carroceria");
}

void WelcomeWidget::onPushButtonBuscarClicked()
{
    QVariantMap filtros;

    auto addFilter = [&](QComboBox *combo, const QString &key){
        if (!combo->currentText().isEmpty())
            filtros[key] = combo->currentText();
    };

    addFilter(ui->comboBoxMarca,       "marca");
    addFilter(ui->comboBoxModelo,      "modelo");
    addFilter(ui->comboBoxEpoca,       "epoca");
    addFilter(ui->comboBoxColor,       "color");
    addFilter(ui->comboBoxMotor,       "motor");
    addFilter(ui->comboBoxCarroceria,  "carroceria");
    addFilter(ui->comboBoxPropietario, "propietario");

    if (!ui->lineEditVin->text().isEmpty())
        filtros["vin"] = ui->lineEditVin->text().trimmed();

    if (!ui->lineEditMatricula->text().isEmpty())
        filtros["placa"] = ui->lineEditMatricula->text().trimmed();

    filtros["estado"] = ui->checkBoxEstado->isChecked();

    emit goDataTableWidgetAndResearchRequested(filtros);
}

void WelcomeWidget::clearSearchFields()
{
    ui->lineEditVin->clear();
    ui->lineEditMatricula->clear();

    auto resetCombo = [&](QComboBox *combo){
        int idx = combo->findText("");
        if (idx < 0) {
            combo->insertItem(0, "");
            idx = 0;
        }
        combo->setCurrentIndex(idx);
    };

    resetCombo(ui->comboBoxMarca);
    resetCombo(ui->comboBoxModelo);
    resetCombo(ui->comboBoxEpoca);
    resetCombo(ui->comboBoxColor);
    resetCombo(ui->comboBoxMotor);
    resetCombo(ui->comboBoxCarroceria);
    resetCombo(ui->comboBoxPropietario);

    ui->checkBoxEstado->setChecked(false);
}
