cmake_minimum_required(VERSION 3.16)
project(Inventario-de-Autos LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Habilita soporte para Qt (ajusta la versión que tengas instalada)
find_package(Qt6 COMPONENTS Widgets REQUIRED Sql Gui Core)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
# Salidas de binarios/librerías

file(MAKE_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
file(MAKE_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

# Incluye tus carpetas de código
add_subdirectory(src)
