cmake_minimum_required(VERSION 3.14)
set(TARGET apni_client)
project(${TARGET} VERSION "0.1.1" LANGUAGES CXX)
configure_file("${PROJECT_SOURCE_DIR}/config.h.in" "${PROJECT_SOURCE_DIR}/config.h")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/Modules")
find_package(ODB REQUIRED COMPONENTS qt sqlite)
include(${ODB_USE_FILE})

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Core Quick DBus REQUIRED)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/Modules")

#get_target_property(JSON_INC_PATH jsoncpp_lib INTERFACE_INCLUDE_DIRECTORIES)
#include_directories(${JSON_INC_PATH})

find_package(PkgConfig REQUIRED)
pkg_check_modules(JSONCPP jsoncpp)
link_libraries(${JSONCPP_LIBRARIES})
include_directories(${JSON_INC_PATH})

include(${ODB_USE_FILE})

file(GLOB CSV_LIB  ./qtcsv/*)
aux_source_directory(. SRC_LIST)
file(GLOB HEAD_LIST  *.h)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/models $ENV{QT_INCLUDE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/qtcsv)

file(GLOB OdbCMake_ODB_HEADERS
        ./models/*.h)

message(STATUS "debug:" ${OdbCMake_ODB_HEADERS})
message(STATUS "debug::qt include:" $ENV{QT_INCLUDE_DIR})

###!!!SCHEMA_NAME APnI should same with src's string.
odb_compile(OdbCMake_OUTPUTS FILES ${OdbCMake_ODB_HEADERS} DB sqlite
    GENERATE_QUERY GENERATE_SESSION GENERATE_PREPARED GENERATE_SCHEMA
    SCHEMA_NAME APnI
    STANDARD c++17
    X_CPP_OPTION -fPIC
    PROFILE qt
    INCLUDE $ENV{QT_INCLUDE_DIR}
    )

set(PROJECT_SOURCES
        qml.qrc
           ${SRC_LIST} ${HEAD_LIST}
           ${OdbCMake_OUTPUTS}
           ${OdbCMake_ODB_HEADERS}
           ${CSV_LIB})


add_executable(${TARGET} ${PROJECT_SOURCES})

target_link_libraries(${TARGET}
        Qt${QT_VERSION_MAJOR}::Core  Qt${QT_VERSION_MAJOR}::Quick Qt5::DBus
        ${ODB_LIBRARIES}
        sqlite3
        ${JSONCPP_LIBRARIES}
        )

target_include_directories(${TARGET}
        PRIVATE
                ${ODB_INCLUDE_DIRS}
                ${ODB_COMPILE_OUTPUT_DIR}
                )

target_compile_definitions(${TARGET}
        PRIVATE
                DATABASE_SQLITE
                PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)
