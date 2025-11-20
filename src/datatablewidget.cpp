#include "datatablewidget.h"
#include "ui_datatablewidget.h"
#include "infodialog.h"
#include "databasemanager.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QPixmap>
#include <QDebug>
#include <QHeaderView>

DataTableWidget::DataTableWidget(QWidget *parent)
    : QWidget(parent),
    ui(new Ui::DataTableWidget),
    m_model(new QSqlQueryModel(this))
{
    ui->setupUi(this);

    // Configuración de la tabla
    ui->tableViewListaDeCarros->setModel(m_model);
    ui->tableViewListaDeCarros->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableViewListaDeCarros->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableViewListaDeCarros->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableViewListaDeCarros->setAlternatingRowColors(true);
    ui->tableViewListaDeCarros->setSortingEnabled(true);
    ui->tableViewListaDeCarros->verticalHeader()->hide();
    ui->tableViewListaDeCarros->horizontalHeader()->setStretchLastSection(true);

    // Cargar valores únicos a los combos
    loadFilters();

    // Tabla inicial
    applyFilters();

    // Conectar selección de fila
    connect(ui->tableViewListaDeCarros->selectionModel(),
            &QItemSelectionModel::currentRowChanged,
            this, &DataTableWidget::onRowSelected);

    // Botón Buscar
    connect(ui->pushButtonBuscar, &QPushButton::clicked, this, &DataTableWidget::applyFilters);

    // Botón Regresar
    connect(ui->pushButtonRegresar, &QPushButton::clicked, this, [this]{
        clearFields();
        emit goWelcomeWidgetRequested();
    });
}

DataTableWidget::~DataTableWidget()
{
    delete ui;
}

// -------------------- FILTROS --------------------
void DataTableWidget::setFilters(const QVariantMap &f)
{
    m_currentFilters = f;

    // Primero cargar valores únicos en combos si no se ha hecho
    loadFilters();

    auto combo = [&](QComboBox *c, const QString &key){
        if (!f.contains(key)) return;
        QString val = f.value(key).toString();
        int idx = c->findText(val);
        if (idx >= 0) c->setCurrentIndex(idx);
        else c->setCurrentIndex(0); // fallback a vacío
    };

    combo(ui->comboBoxMarca,       "marca");
    combo(ui->comboBoxModelo,      "modelo");
    combo(ui->comboBoxEpoca,       "epoca");
    combo(ui->comboBoxColor,       "color");
    combo(ui->comboBoxPropietario, "propietario");
    combo(ui->comboBoxMotor,       "motor");
    combo(ui->comboBoxCarroceria,  "carroceria");

    ui->lineEditVin->setText(f.value("vin").toString());
    ui->lineEditMatricula->setText(f.value("placa").toString());

    if (f.contains("estado"))
        ui->checkBoxEstado->setChecked(f.value("estado").toBool());

    // Aplica los filtros
    applyFilters();
}

void DataTableWidget::applyFilters()
{
    QString sql =
        "SELECT v.vin, v.marca, v.modelo, v.epoca, v.color, "
        "v.placa, v.propietario, v.estado, "
        "d.motor, d.carroceria, d.kilometraje, d.puertas "
        "FROM vehiculos v "
        "LEFT JOIN datos_tecnicos d ON v.vin = d.vin ";

    QStringList where;
    QList<QVariant> args;

    auto add = [&](const QString &col, const QVariant &val){
        if (!val.toString().trimmed().isEmpty())
        {
            where << col + " = ?";
            args << val;
        }
    };

    add("v.vin",            ui->lineEditVin->text());
    add("v.placa",          ui->lineEditMatricula->text());
    add("v.marca",          ui->comboBoxMarca->currentText());
    add("v.modelo",         ui->comboBoxModelo->currentText());
    add("v.epoca",          ui->comboBoxEpoca->currentText());
    add("v.color",          ui->comboBoxColor->currentText());
    add("v.propietario",    ui->comboBoxPropietario->currentText());
    add("d.motor",          ui->comboBoxMotor->currentText());
    add("d.carroceria",     ui->comboBoxCarroceria->currentText());

    if (ui->checkBoxEstado->isChecked())
        add("v.estado", 1);

    if (!where.isEmpty())
        sql += " WHERE " + where.join(" AND ");

    sql += " ORDER BY v.marca, v.modelo";

    QSqlQuery query(DatabaseManager::instance().getDatabase());
    query.prepare(sql);

    for (const QVariant &v : args)
        query.addBindValue(v);

    if (!query.exec()) {
        qDebug() << "SQL ERROR:" << query.lastError() << "QUERY:" << sql;
        return;
    }

    m_model->setQuery(std::move(query));

    if (m_model->lastError().isValid()) {
        qDebug() << "Model error:" << m_model->lastError();
        return;
    }

    const QStringList headers = {
        "VIN", "Marca", "Modelo", "Época", "Color",
        "Placa", "Propietario", "Estado",
        "Motor", "Carrocería", "KM", "Puertas"
    };

    for (int i = 0; i < headers.size(); ++i)
        m_model->setHeaderData(i, Qt::Horizontal, headers[i]);

    ui->tableViewListaDeCarros->resizeColumnsToContents();

    // Seleccionar primera fila automáticamente
    if (m_model->rowCount() > 0)
    {
        QModelIndex firstIndex = m_model->index(0, 0);
        ui->tableViewListaDeCarros->setCurrentIndex(firstIndex);
        onRowSelected(firstIndex, QModelIndex());
    }
    else
    {
        ui->labelImagen->setText("Sin imagen");
    }
}

// -------------------- COMBOS --------------------
void DataTableWidget::loadFilters()
{
    auto fill = [&](QComboBox *c, const QString &table, const QString &col){
        QString old = c->currentText();
        c->blockSignals(true);
        c->clear();
        c->addItem("");

        QSqlQuery q(DatabaseManager::instance().getDatabase());
        QString str = QString("SELECT DISTINCT %1 FROM %2 WHERE %1 IS NOT NULL AND %1 != '' ORDER BY %1")
                          .arg(col, table);
        if (q.exec(str))
        {
            while (q.next())
            {
                c->addItem(q.value(0).toString());
            }
        } else {
            qDebug() << "ERROR FILTER QUERY:" << q.lastError();
        }

        int idx = c->findText(old);
        c->setCurrentIndex(idx >= 0 ? idx : 0);
        c->blockSignals(false);
    };

    fill(ui->comboBoxMarca,       "vehiculos", "marca");
    fill(ui->comboBoxModelo,      "vehiculos", "modelo");
    fill(ui->comboBoxEpoca,       "vehiculos", "epoca");
    fill(ui->comboBoxColor,       "vehiculos", "color");
    fill(ui->comboBoxPropietario, "vehiculos", "propietario");
    fill(ui->comboBoxMotor,       "datos_tecnicos", "motor");
    fill(ui->comboBoxCarroceria,  "datos_tecnicos", "carroceria");
}

// -------------------- LIMPIAR CAMPOS --------------------
void DataTableWidget::clearFields()
{
    ui->lineEditVin->clear();
    ui->lineEditMatricula->clear();

    auto reset = [&](QComboBox *c){ c->setCurrentIndex(0); };

    reset(ui->comboBoxMarca);
    reset(ui->comboBoxModelo);
    reset(ui->comboBoxEpoca);
    reset(ui->comboBoxColor);
    reset(ui->comboBoxPropietario);
    reset(ui->comboBoxMotor);
    reset(ui->comboBoxCarroceria);

    ui->checkBoxEstado->setChecked(false);
}

// -------------------- IMAGEN --------------------
void DataTableWidget::onRowSelected(const QModelIndex &current, const QModelIndex &)
{
    if (!current.isValid()) return;

    QString vin = m_model->data(m_model->index(current.row(), 0)).toString();

    QSqlQuery q(DatabaseManager::instance().getDatabase());
    q.prepare("SELECT foto FROM vehiculos WHERE vin = ?");
    q.addBindValue(vin);

    if (!q.exec() || !q.next())
    {
        ui->labelImagen->setText("Sin imagen");
        return;
    }

    QPixmap px;
    px.loadFromData(q.value(0).toByteArray());

    if (px.isNull())
    {
        ui->labelImagen->setText("Sin imagen");
        return;
    }

    ui->labelImagen->setPixmap(px.scaled(ui->labelImagen->size(),
                                         Qt::KeepAspectRatio,
                                         Qt::SmoothTransformation));
}

// -------------------- INFO DIALOG --------------------
// src/datatablewidget.cpp (fragmento)
void DataTableWidget::abrirInfoDialog()
{
    QModelIndex index = ui->tableViewListaDeCarros->currentIndex();
    if (!index.isValid()) return;

    // VIN está en la columna 0 del modelo SQL actual
    QString vin = m_model->data(m_model->index(index.row(), 0)).toString();

    InfoDialog dlg(this);
    dlg.setVehicleData(vin);

    // Conectar señales para refrescar la tabla si hubo cambios
    connect(&dlg, &InfoDialog::vehicleUpdated, this, [this](const QString &){
        this->applyFilters();
    });
    connect(&dlg, &InfoDialog::vehicleDeleted, this, [this](const QString &){
        this->applyFilters();
    });

    // Abrir modal
    dlg.exec();
}

void DataTableWidget::on_pushButtonDetalles_clicked()
{
    abrirInfoDialog();
}
