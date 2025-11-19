#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QString>
#include <QSqlDatabase>

class DatabaseManager {
public:
    // Singleton → una sola instancia controla la BD
    static DatabaseManager& instance();

    bool initialize();                      // Abre la BD y crea tablas
    QSqlDatabase& getDatabase();            // Obtener referencia a la BD

private:
    DatabaseManager();                      // Constructor privado
    bool createTables();                    // Ejecuta el schema.sql

    QSqlDatabase db;
};

#endif
