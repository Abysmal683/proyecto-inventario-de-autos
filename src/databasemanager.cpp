#include "databasemanager.h"
#include <QSqlQuery>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QSqlError>
DatabaseManager::DatabaseManager() {
    // Usamos SQLite con Qt
    db = QSqlDatabase::addDatabase("QSQLITE");

    // Archivo de base de datos local
    db.setDatabaseName("inventario_autos.db");
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

    qDebug() << "Base de datos abierta correctamente.";

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
