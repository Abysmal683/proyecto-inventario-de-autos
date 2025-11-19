#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QString>
#include <QSqlDatabase>
#include <QVariantMap>
#include <QList>
class DatabaseManager {
public:
    // Singleton → una sola instancia controla la BD
    static DatabaseManager& instance();

    bool initialize();                      // Abre la BD y crea tablas
    QSqlDatabase& getDatabase();            // Obtener referencia a la BD
    // Vehículos
    bool addVehicle(const QString& vin, const QString& marca, const QString& modelo, int epoca,
                    const QString& color, const QString& placa, const QString& propietario,
                    const QByteArray& foto, const bool estado);

    QVariantMap getVehicle(const QString& vin);    // Devuelve un mapa con todos los campos
    QList<QVariantMap> getAllVehicles();

    bool updateVehicle(const QString& vin, const QVariantMap& campos);
    bool deleteVehicle(const QString& vin);
    // Datos técnicos
    bool addTechnicalData(const QString& vin, const QString& motor, int kilometraje, int puertas,
                          const QString& carroceria, const QString& detalles_adicionales);

    QVariantMap getTechnicalData(const QString& vin);
    QList<QVariantMap> getAllTechnicalData();
    bool updateTechnicalData(int id, const QVariantMap& campos);
    bool deleteTechnicalData(int id);
private:
    DatabaseManager();                      // Constructor privado
    bool createTables();                    // Ejecuta el schema.sql

    QSqlDatabase db;
};

#endif
