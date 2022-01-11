cmake_minimum_required(VERSION 3.14)

project(apai LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


find_package(QT NAMES Qt6 Qt5 COMPONENTS Core Quick REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Quick REQUIRED)

aux_source_directory(. SRC_LIST)
file(GLOB HEAD_LIST  *.h *.hxx)
file(GLOB DATA_SRC_LIST data/*.cpp)

set(PROJECT_SOURCES
        qml.qrc
           ${SRC_LIST} ${HEAD_LIST})


add_definitions( -DDATABASE_SQLITE)

add_executable(apai ${PROJECT_SOURCES})


target_compile_definitions(apai
  PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)
target_link_libraries(apai
  PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Quick
  odb
  odb-sqlite
  odb-qt
)
