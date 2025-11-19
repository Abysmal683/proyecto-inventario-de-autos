-- =====================================================================
-- TABLA: vehiculos
-- Datos generales del vehículo
-- =====================================================================

CREATE TABLE IF NOT EXISTS vehiculos (
    vin TEXT PRIMARY KEY
        CHECK (length(vin) = 17),     -- VIN debe tener 17 caracteres

    marca TEXT NOT NULL,              -- Marca (ej: Honda)
    modelo TEXT NOT NULL,             -- Modelo (ej: Civic)

    epoca INTEGER
        CHECK (epoca >= 1886),        -- Año mínimo aceptado

    color TEXT,                       -- Color descriptivo

    placa TEXT UNIQUE NOT NULL,       -- Matrícula única

    propietario TEXT NOT NULL,        -- Dueño del vehículo

    foto BLOB,                        -- Foto en binario
    estado INTEGER NOT NULL
        CHECK (estado IN (0, 1)) -- 0 = Retirado, 1 = Almacenado
);


-- =====================================================================
-- TABLA: datos_tecnicos
-- Información más detallada del vehículo
-- =====================================================================

CREATE TABLE IF NOT EXISTS datos_tecnicos (
    id INTEGER PRIMARY KEY AUTOINCREMENT,  -- ID interno

    vin TEXT NOT NULL,                     -- Relación con vehiculos

    motor TEXT NOT NULL
        CHECK (motor IN ('Gasolina','Diesel','Eletrico','Hibrido')),

    kilometraje INTEGER
        CHECK (kilometraje >= 0),

    puertas INTEGER
        CHECK (puertas >= 0 AND puertas <= 12),

    carroceria TEXT NOT NULL,              -- Tipo (sedán, hatchback, etc)

    detalles_adicionales TEXT,             -- Info opcional

    FOREIGN KEY (vin) REFERENCES vehiculos (vin)
        ON DELETE CASCADE                  -- Si un vehículo se elimina, sus datos técnicos también
);
