cmake_minimum_required(VERSION 3.1)
project(Inventario-de-Autos LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Habilita soporte para Qt (ajusta la versión que tengas instalada)
find_package(Qt6 COMPONENTS Widgets REQUIRED)

# Incluye tus carpetas de código
include_directories(include)
add_subdirectory(src)

# Salidas de binarios/librerías
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
