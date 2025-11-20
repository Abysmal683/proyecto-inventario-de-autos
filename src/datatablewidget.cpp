#include "datatablewidget.h"
#include "ui_datatablewidget.h"
#include "infodialog.h"
#include "databasemanager.h"

#include <QSqlQueryModel>
#include <QSqlQuery>
#include <QPixmap>
#include <QDebug>
#include <QSqlError>

/* ===========================================================
 * Constructor
 * =========================================================== */
DataTableWidget::DataTableWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::DataTableWidget)
    , m_model(new QSqlQueryModel(this))
{
    ui->setupUi(this);

    /* ---- CONFIG TABLA ---- */
    ui->tableViewListaDeCarros->setModel(m_model);
    ui->tableViewListaDeCarros->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableViewListaDeCarros->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableViewListaDeCarros->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableViewListaDeCarros->setAlternatingRowColors(true);
    ui->tableViewListaDeCarros->setSortingEnabled(true);
    ui->tableViewListaDeCarros->verticalHeader()->hide();
    ui->tableViewListaDeCarros->horizontalHeader()->setStretchLastSection(true);

    /* ---- Cargar combos desde BD ---- */
    loadFilters();

    /* ---- Cargar la tabla inicial ---- */
    applyFilters();

    connect(ui->tableViewListaDeCarros->selectionModel(),
            &QItemSelectionModel::currentRowChanged,
            this, &DataTableWidget::onRowSelected);

    /* ---- Botón Buscar ---- */
    connect(ui->pushButtonBuscar, &QPushButton::clicked,
            this, &DataTableWidget::applyFilters);

    /* ---- Regresar ---- */
    connect(ui->pushButtonRegresar, &QPushButton::clicked, this, [this]{
        clearFields();
        emit goWelcomeWidgetRequested();
    });
}

DataTableWidget::~DataTableWidget()
{
    delete ui;
}

/* ===========================================================
 * Botón detalles
 * =========================================================== */
void DataTableWidget::on_pushButtonDetalles_clicked()
{
    QModelIndex idx = ui->tableViewListaDeCarros->currentIndex();
    if (!idx.isValid())
        return;

    InfoDialog infoDialog(this);
    infoDialog.setWindowTitle("Detalles - Extras");
    infoDialog.exec();
}

/* ===========================================================
 * Recibir filtros desde WelcomeWidget
 * =========================================================== */
void DataTableWidget::setFilters(const QVariantMap &f)
{
    auto combo = [&](QComboBox *c, const QString &key){
        if (!f.contains(key)) return;
        QString val = f.value(key).toString();
        int idx = c->findText(val);
        if (idx >= 0) c->setCurrentIndex(idx);
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

    applyFilters();   // ⭐ FIX: antes no aplicaba los filtros recibidos
}

/* ===========================================================
 * Aplicar filtros
 * =========================================================== */
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
        if (!val.toString().trimmed().isEmpty()) {
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

    /* ---- Ejecutar consulta ---- */
    QSqlQuery query(DatabaseManager::instance().getDatabase());
    query.prepare(sql);

    for (const QVariant &v : args)
        query.addBindValue(v);

    if (!query.exec()) {
        qDebug() << "SQL ERROR:" << query.lastError();
        qDebug() << "QUERY:" << sql;
        return;
    }

    m_model->setQuery(std::move(query));

    if (m_model->lastError().isValid()) {
        qDebug() << "Model error:" << m_model->lastError();
        return;
    }

    qDebug() << "FILAS CARGADAS:" << m_model->rowCount();

    /* ---- Encabezados ---- */
    const QStringList headers = {
        "VIN", "Marca", "Modelo", "Época", "Color",
        "Placa", "Propietario", "Estado",
        "Motor", "Carrocería", "KM", "Puertas"
    };

    for (int i = 0; i < headers.size(); ++i)
        m_model->setHeaderData(i, Qt::Horizontal, headers[i]);

    ui->tableViewListaDeCarros->resizeColumnsToContents();

    connect(ui->tableViewListaDeCarros->selectionModel(),
            &QItemSelectionModel::currentRowChanged,
            this, &DataTableWidget::onRowSelected);
    /* ---- Seleccionar automáticamente la primera fila si existe ---- */
    if (m_model->rowCount() > 0) {
        QModelIndex firstIndex = m_model->index(0, 0);
        ui->tableViewListaDeCarros->setCurrentIndex(firstIndex);
        onRowSelected(firstIndex, QModelIndex()); // disparar carga de imagen
    } else {
        ui->labelImagen->setText("Sin imagen"); // si no hay filas
    }
}

/* ===========================================================
 * Cargar combos con valores DISTINCT
 * =========================================================== */
void DataTableWidget::loadFilters()
{
    auto fill = [&](QComboBox *c, const QString &table, const QString &col){
        QString old = c->currentText();           // ← preserva valor
        c->blockSignals(true);                     // ← bloquea señales
        c->clear();
        c->addItem("");

        QSqlQuery q(DatabaseManager::instance().getDatabase());
        QString str = QString("SELECT DISTINCT %1 FROM %2 "
                              "WHERE %1 IS NOT NULL AND %1 != '' ORDER BY %1")
                          .arg(col, table);

        if (!q.exec(str)) {
            qDebug() << "ERROR FILTER QUERY:" << q.lastError();
        } else {
            while (q.next()) {
                QString val = q.value(0).toString();
                if (!val.isEmpty())
                    c->addItem(val);
            }
        }

        int idx = c->findText(old);               // ← reestablece valor previo
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

/* ===========================================================
 * Selección de fila → cargar imagen
 * =========================================================== */
void DataTableWidget::onRowSelected(const QModelIndex &current, const QModelIndex &)
{
    if (!current.isValid()) return;

    QString vin = m_model->data(m_model->index(current.row(), 0)).toString();

    QSqlQuery q(DatabaseManager::instance().getDatabase());
    q.prepare("SELECT foto FROM vehiculos WHERE vin = ?");
    q.addBindValue(vin);

    if (!q.exec() || !q.next()) {
        ui->labelImagen->setText("Sin imagen");
        return;
    }

    QPixmap px;
    px.loadFromData(q.value(0).toByteArray());

    if (px.isNull()) {
        ui->labelImagen->setText("Sin imagen");
        return;
    }

    ui->labelImagen->setPixmap(
        px.scaled(ui->labelImagen->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation)
        );
}

/* ===========================================================
 * Limpiar campos
 * =========================================================== */
void DataTableWidget::clearFields()
{
    ui->lineEditVin->clear();
    ui->lineEditMatricula->clear();

    auto reset = [&](QComboBox *c){
        c->setCurrentIndex(0);
     //   c->addItem("");
    };

    reset(ui->comboBoxMarca);
    reset(ui->comboBoxModelo);
    reset(ui->comboBoxEpoca);
    reset(ui->comboBoxColor);
    reset(ui->comboBoxPropietario);
    reset(ui->comboBoxMotor);
    reset(ui->comboBoxCarroceria);

    ui->checkBoxEstado->setChecked(false);
}
