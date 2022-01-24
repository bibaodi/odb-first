cmake_minimum_required(VERSION 3.14)
set(TARGET apni_client)
project(${TARGET} LANGUAGES CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/Modules")
find_package(ODB REQUIRED COMPONENTS qt sqlite)
include(${ODB_USE_FILE})

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


find_package(QT NAMES Qt6 Qt5 COMPONENTS Core Quick REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Quick REQUIRED)

set(ODB_FILE_PATH ${CMAKE_CURRENT_SOURCE_DIR})

aux_source_directory(. SRC_LIST)
file(GLOB HEAD_LIST  *.h)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/Modules")

find_package(ODB REQUIRED COMPONENTS qt sqlite OPTIONAL_COMPONENTS )
include(${ODB_USE_FILE})

set(OdbCMake_SOURCES
        main.cpp
        database_create.h)

set(OdbCMake_ODB_HEADERS
        person.h)

odb_compile(OdbCMake_SOURCES FILES ${OdbCMake_ODB_HEADERS} DB sqlite
    GENERATE_QUERY GENERATE_SESSION GENERATE_PREPARED GENERATE_SCHEMA
    SCHEMA_NAME etons
    STANDARD c++11
    #PROFILE qt
    X_CPP_OPTION -fPIC
    )

set(PROJECT_SOURCES
        qml.qrc
           ${SRC_LIST} ${HEAD_LIST}
           ${OdbCMake_SOURCES}
           ${OdbCMake_ODB_HEADERS})


add_executable(${TARGET} ${PROJECT_SOURCES})

target_link_libraries(${TARGET}
        ${ODB_LIBRARIES}
        Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Quick)

target_include_directories(${TARGET}
        PRIVATE
                ${ODB_INCLUDE_DIRS}
                ${ODB_COMPILE_OUTPUT_DIR})

target_compile_definitions(${TARGET}
        PRIVATE
                DATABASE_SQLITE
                PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)
