set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_AUTOUIC_SEARCH_PATHS
    ${CMAKE_SOURCE_DIR}/ui
)

# NO declarar más RESOURCES después
qt_add_resources(APP_RESOURCES
    ${CMAKE_SOURCE_DIR}/resources/resources.qrc
)

file(GLOB_RECURSE CPP_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

file(GLOB_RECURSE HEADER_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_SOURCE_DIR}/include/*.h
)
find_package(Qt6 COMPONENTS Widgets REQUIRED Sql Gui Core)

add_executable(${PROJECT_NAME}
    ${CPP_SOURCES}
    ${HEADER_SOURCES}
    ${APP_RESOURCES}
)


target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Sql)
