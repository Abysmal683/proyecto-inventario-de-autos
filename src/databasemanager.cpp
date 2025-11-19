#include "databasemanager.h"
#include <QSqlQuery>
#include <QFile>
#include <QTextStream>
#include <QSqlError>
#include <QDir>
#include <QStandardPaths>
#include <QFileInfo>
#include <QSqlRecord>

/*
QString dbPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/inventario_autos.db";
QDir dir(QFileInfo(dbPath).path());
if (!dir.exists()) dir.mkpath(dir.path());
*/
DatabaseManager::DatabaseManager() {
    // Usamos SQLite con Qt
    db = QSqlDatabase::addDatabase("QSQLITE");
    QString dbPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/inventario_autos.db";
    QDir dir(QFileInfo(dbPath).path());;
    if (!dir.exists()) dir.mkpath(dir.path());
    // Archivo de base de datos local
    db.setDatabaseName(dbPath);
}

DatabaseManager& DatabaseManager::instance() {
    static DatabaseManager instance;
    return instance;
}

bool DatabaseManager::initialize() {
    if (!db.open()) {
        qCritical() << "ERROR al abrir la base de datos:" << db.lastError().text();
        return false;
    }

    return createTables();
}

bool DatabaseManager::createTables() {
    QFile schemaFile(":/SQL/inicializacion_inventario_de_autos.sql");
    // Acceso a /SQL/inicializacion_inventario_de_autos.qrc

    if (!schemaFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qCritical() << "No se pudo abrir inicializacion_inventario_de_autos.sql desde recursos.";
        return false;
    }

    QTextStream stream(&schemaFile);
    QString sqlContent = stream.readAll();
    schemaFile.close();

    QSqlQuery query;
    const QStringList commands = sqlContent.split(';', Qt::SkipEmptyParts);

    for (const QString& cmd : commands) {
        QString trimmed = cmd.trimmed();
        if (trimmed.isEmpty())
            continue;

        if (!query.exec(trimmed)) {
            qCritical() << "Error ejecutando SQL:" << trimmed;
            qCritical() << "Detalle:" << query.lastError().text();
            return false;
        }
    }

    qDebug() << "Tablas creadas/verificadas correctamente.";
    return true;
}

QSqlDatabase& DatabaseManager::getDatabase() {
    return db;
}
// Cambiar el tipo de estado a bool
bool DatabaseManager::addVehicle(const QString& vin, const QString& marca, const QString& modelo, int epoca,
                                 const QString& color, const QString& placa, const QString& propietario,
                                 const QByteArray& foto, bool estado) // <- CAMBIO A BOOL
{
    QSqlQuery query(db);
    query.prepare(R"(
        INSERT INTO vehiculos (vin, marca, modelo, epoca, color, placa, propietario, foto, estado)
        VALUES (:vin, :marca, :modelo, :epoca, :color, :placa, :propietario, :foto, :estado)
    )");

    query.bindValue(":vin", vin);
    query.bindValue(":marca", marca);
    query.bindValue(":modelo", modelo);
    query.bindValue(":epoca", epoca);
    query.bindValue(":color", color);
    query.bindValue(":placa", placa);
    query.bindValue(":propietario", propietario);
    query.bindValue(":foto", foto);

    // Convertimos bool a int para la BD (0 = Retirado, 1 = Almacenado)
    query.bindValue(":estado", estado ? 1 : 0);

    if (!query.exec()) {
        qCritical() << "Error insertando vehículo:" << query.lastError().text();
        return false;
    }
    return true;
}

QVariantMap DatabaseManager::getVehicle(const QString& vin)
{
    QVariantMap result;
    QSqlQuery query(db);
    query.prepare("SELECT * FROM vehiculos WHERE vin = :vin");
    query.bindValue(":vin", vin);

    if (query.exec() && query.next()) {
        QSqlRecord rec = query.record();
        for (int i = 0; i < rec.count(); ++i)
        {
            if (rec.fieldName(i) == "estado")
                result["estado"] = query.value(i).toInt() == 1;
            else
                result[rec.fieldName(i)] = query.value(i);
        }
    }
    return result;
}
QList<QVariantMap> DatabaseManager::getAllVehicles()
{
    QList<QVariantMap> list;
    QSqlQuery query(db);

    if (query.exec("SELECT * FROM vehiculos")) {
        while (query.next()) {
            QVariantMap map;
            QSqlRecord rec = query.record();
            for (int i = 0; i < rec.count(); ++i) {
                if (rec.fieldName(i) == "estado") {
                    map["estado"] = query.value(i).toInt() == 1; // <- CAMBIO
                } else {
                    map[rec.fieldName(i)] = query.value(i);
                }
            }
            list.append(map);
        }
    }
    return list;
}
bool DatabaseManager::updateVehicle(const QString& vin, const QVariantMap& campos)
{
    if (campos.isEmpty()) return false;

    QStringList updates;
    for (auto it = campos.begin(); it != campos.end(); ++it)
        updates.append(it.key() + " = :" + it.key());

    QString sql = "UPDATE vehiculos SET " + updates.join(", ") + " WHERE vin = :vin";

    QSqlQuery query(db);
    query.prepare(sql);
    query.bindValue(":vin", vin);

    for (auto it = campos.begin(); it != campos.end(); ++it) {
        if (it.key() == "estado") {
            query.bindValue(":" + it.key(), it.value().toBool() ? 1 : 0);
        } else {
            query.bindValue(":" + it.key(), it.value());
        }
    }
    // FALTA: Ejecutar query
    if (!query.exec()) {
        qCritical() << "Error actualizando vehículo:" << query.lastError().text();
        return false;
    }
    return true;
}
bool DatabaseManager::deleteVehicle(const QString& vin)
{
    QSqlQuery query(db);
    query.prepare("DELETE FROM vehiculos WHERE vin = :vin");
    query.bindValue(":vin", vin);

    if (!query.exec()) {
        qCritical() << "Error eliminando vehículo:" << query.lastError().text();
        return false;
    }
    return true;
}
//datos tecnicos
bool DatabaseManager::addTechnicalData(const QString& vin, const QString& motor, int kilometraje, int puertas,
                                       const QString& carroceria, const QString& detalles_adicionales)
{
    QSqlQuery query(db);
    query.prepare(R"(
        INSERT INTO datos_tecnicos (vin, motor, kilometraje, puertas, carroceria, detalles_adicionales)
        VALUES (:vin, :motor, :kilometraje, :puertas, :carroceria, :detalles_adicionales)
    )");

    query.bindValue(":vin", vin);
    query.bindValue(":motor", motor);
    query.bindValue(":kilometraje", kilometraje);
    query.bindValue(":puertas", puertas);
    query.bindValue(":carroceria", carroceria);
    query.bindValue(":detalles_adicionales", detalles_adicionales);

    if (!query.exec()) {
        qCritical() << "Error insertando datos técnicos:" << query.lastError().text();
        return false;
    }
    return true;
}

QVariantMap DatabaseManager::getTechnicalData(const QString& vin)
{
    QVariantMap result;
    QSqlQuery query(db);
    query.prepare("SELECT * FROM datos_tecnicos WHERE vin = :vin");
    query.bindValue(":vin", vin);

    if (query.exec() && query.next()) {
        QSqlRecord rec = query.record();
        for (int i = 0; i < rec.count(); ++i)
            result[rec.fieldName(i)] = query.value(i);
    }
    return result;
}

QList<QVariantMap> DatabaseManager::getAllTechnicalData()
{
    QList<QVariantMap> list;
    QSqlQuery query(db);

    if (query.exec("SELECT * FROM datos_tecnicos")) {
        while (query.next()) {
            QVariantMap map;
            QSqlRecord rec = query.record();
            for (int i = 0; i < rec.count(); ++i)
                map[rec.fieldName(i)] = query.value(i);
            list.append(map);
        }
    }
    return list;
}

bool DatabaseManager::updateTechnicalData(int id, const QVariantMap& campos)
{
    if (campos.isEmpty()) return false;

    QStringList updates;
    for (auto it = campos.begin(); it != campos.end(); ++it)
        updates.append(it.key() + " = :" + it.key());

    QString sql = "UPDATE datos_tecnicos SET " + updates.join(", ") + " WHERE id = :id";

    QSqlQuery query(db);
    query.prepare(sql);
    query.bindValue(":id", id);

    for (auto it = campos.begin(); it != campos.end(); ++it)
        query.bindValue(":" + it.key(), it.value());

    if (!query.exec()) {
        qCritical() << "Error actualizando datos técnicos:" << query.lastError().text();
        return false;
    }
    return true;
}

bool DatabaseManager::deleteTechnicalData(int id)
{
    QSqlQuery query(db);
    query.prepare("DELETE FROM datos_tecnicos WHERE id = :id");
    query.bindValue(":id", id);

    if (!query.exec()) {
        qCritical() << "Error eliminando datos técnicos:" << query.lastError().text();
        return false;
    }
    return true;
}
