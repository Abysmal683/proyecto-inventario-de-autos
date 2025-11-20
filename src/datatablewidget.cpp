#include "datatablewidget.h"
#include "ui_datatablewidget.h"
#include "infodialog.h"
#include "databasemanager.h"
#include <QSqlQueryModel>
#include <QSqlQuery>
DataTableWidget::DataTableWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::DataTableWidget)
{
    ui->setupUi(this);
    loadFilters();
    connect(ui->pushButtonRegresar, &QPushButton::clicked, this, [=](){
        clearFields();
        emit goWelcomeWidgetRequested();
    });

}

DataTableWidget::~DataTableWidget()
{
    delete ui;
}
void DataTableWidget::on_pushButtonDetalles_clicked(){
    InfoDialog infoDialog(this);
    infoDialog.setWindowTitle("Detalles-Extras");
    infoDialog.exec();
}
void DataTableWidget::setFilters(const QVariantMap &filtros)
{
    auto setCombo = [&](QComboBox *c, const QString &key){
        if (!c) return;
        if (!filtros.contains(key)) return;

        QString val = filtros[key].toString().trimmed();
        if (val.isEmpty()) return;

        int idx = c->findText(val);
        if (idx >= 0) c->setCurrentIndex(idx);
    };

    // Combos de vehiculos
    setCombo(ui->comboBoxMarca,       "marca");
    setCombo(ui->comboBoxModelo,      "modelo");
    setCombo(ui->comboBoxEpoca,       "epoca");
    setCombo(ui->comboBoxColor,       "color");
    setCombo(ui->comboBoxPropietario, "propietario");

    // Combos de datos técnicos
    setCombo(ui->comboBoxMotor,       "motor");
    setCombo(ui->comboBoxCarroceria,  "carroceria");

    // Line edits
    if (filtros.contains("vin"))
        ui->lineEditVin->setText(filtros["vin"].toString());

    if (filtros.contains("placa"))
        ui->lineEditMatricula->setText(filtros["placa"].toString());

    // Checkbox
    if (filtros.contains("estado"))
        ui->checkBoxEstado->setChecked(filtros["estado"].toBool());
}
void DataTableWidget::applyFilters()
{
    QString queryStr =
        "SELECT "
        "vehiculos.vin, "
        "vehiculos.marca, "
        "vehiculos.modelo, "
        "vehiculos.epoca, "
        "vehiculos.color, "
        "vehiculos.placa, "
        "vehiculos.propietario, "
        "vehiculos.estado, "
        "datos_tecnicos.motor, "
        "datos_tecnicos.carroceria, "
        "datos_tecnicos.kilometraje, "
        "datos_tecnicos.puertas "
        "FROM vehiculos "
        "LEFT JOIN datos_tecnicos ON vehiculos.vin = datos_tecnicos.vin ";

    QStringList conditions;
    QVariantList params;

    auto addIf = [&](const QString &col, const QVariant &val){
        if (val.isNull()) return;
        if (val.toString().trimmed().isEmpty()) return;
        conditions << QString("%1 = ?").arg(col);
        params << val;
    };

    addIf("vehiculos.vin",        ui->lineEditVin->text());
    addIf("vehiculos.placa",      ui->lineEditMatricula->text());
    addIf("vehiculos.marca",      ui->comboBoxMarca->currentText());
    addIf("vehiculos.modelo",     ui->comboBoxModelo->currentText());
    addIf("vehiculos.epoca",      ui->comboBoxEpoca->currentText());
    addIf("vehiculos.color",      ui->comboBoxColor->currentText());
    addIf("vehiculos.propietario",ui->comboBoxPropietario->currentText());

    // Estado solo se filtra si está activo
    if (ui->checkBoxEstado->isChecked())
        addIf("vehiculos.estado", 1);

    // Datos técnicos
    addIf("datos_tecnicos.motor",      ui->comboBoxMotor->currentText());
    addIf("datos_tecnicos.carroceria", ui->comboBoxCarroceria->currentText());
    if (!conditions.isEmpty())
        queryStr += " WHERE " + conditions.join(" AND ");

    queryStr += " ORDER BY vehiculos.marca, vehiculos.modelo";

    if (!m_model)
        m_model = new QSqlQueryModel(this);

    QSqlQuery q(DatabaseManager::instance().getDatabase());
    q.prepare(queryStr);

    for (const QVariant &v : params)
        q.addBindValue(v);

    m_model->setQuery(std::move(q));
    ui->tableViewListaDeCarros->setModel(m_model);
}

void DataTableWidget::clearFields()
{
    ui->lineEditVin->clear();
    ui->lineEditMatricula->clear();
    auto resetCombo = [&](QComboBox *c){
        int idx = c->findText("");
        if (idx >= 0)
            c->setCurrentIndex(idx);
        else {
            c->insertItem(0, "");
            c->setCurrentIndex(0);
        }
    };

    resetCombo(ui->comboBoxPropietario);
    resetCombo(ui->comboBoxMarca);
    resetCombo(ui->comboBoxModelo);
    resetCombo(ui->comboBoxEpoca);
    resetCombo(ui->comboBoxColor);
    resetCombo(ui->comboBoxMotor);
    resetCombo(ui->comboBoxCarroceria);

    ui->spinBoxKilometraje->setValue(0);
    ui->spinBoxPuertas->setValue(0);
    ui->checkBoxEstado->setChecked(false);
}

void DataTableWidget::loadFilters()
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
    fillCombo(ui->comboBoxPropietario, "vehiculos", "propietario");
    fillCombo(ui->comboBoxMarca,       "vehiculos", "marca");
    fillCombo(ui->comboBoxModelo,      "vehiculos", "modelo");
    fillCombo(ui->comboBoxEpoca,       "vehiculos", "epoca");
    fillCombo(ui->comboBoxColor,       "vehiculos", "color");

    // Datos técnicos
    fillCombo(ui->comboBoxMotor,       "datos_tecnicos", "motor");
    fillCombo(ui->comboBoxCarroceria,  "datos_tecnicos", "carroceria");
}
