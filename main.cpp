// file      : hello/driver.cxx
// copyright : not copyrighted - public domain

#include <iostream>
#include <memory> // std::shared_ptr

#include <odb/database.hxx>
#include <odb/transaction.hxx>

#include "database_create.h" // create_database

#include "apai_db_adapter.h"
#include "person-odb.hxx"
#include "person.h"

#include <QGuiApplication>
#include <QQmlApplicationEngine>

//--csv.begin
#include "reader.h"
#include "stringdata.h"
#include "writer.h"
//--csv.begin.end
#include <vector>

int main(int argc, char *argv[]) {
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(
        &engine, &QQmlApplicationEngine::objectCreated, &app,
        [url](QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        },
        Qt::QueuedConnection);
    engine.load(url);
    // main_db(argc, argv);
    APAI_DB_Adapter ada("testapai.db");
    ada.addVersion("init by eton when testing.");
    ada.addVersion("init by eton when testing2.");

    QList<QStringList> readData = QtCSV::Reader::readToList("/tmp/modes.csv");
    for (int i = 0; i < readData.size(); ++i) {
        qDebug() << readData.at(i).join(",");
    }
    std::vector<QStringList> _rows = readData.toVector().toStdVector();
    ada.addModes(_rows);
    ada.addRows(_rows, APAI_DB_Adapter::TABLE_APOD);
    std::vector<int> _ids;
    std::vector<QString> _names;
    return app.exec();
}
