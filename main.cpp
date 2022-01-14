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

#define APAI_STATIC_TABLE_COUNTS 4

bool generate_db() {
    bool success[APAI_STATIC_TABLE_COUNTS] = {false};
    APAI_DB_Adapter ada("testapai.db");
    ada.addVersion("init by eton when testing.");

    QString csv_files[APAI_STATIC_TABLE_COUNTS] = {QString("probes"), QString("modes"), QString("apods"),
                                                   QString("pulses")};

    for (int i = 0; i < APAI_STATIC_TABLE_COUNTS; i++) {
        QList<QStringList> csv_datas = QtCSV::Reader::readToList(QString("/tmp/%1.csv").arg(csv_files[i]));
        //        for (int i = 0; i < csv_datas.size(); ++i) {
        //            qDebug() << csv_datas.at(i).join(",");
        //        }
        if (csv_datas.length() < 1) {
            success[i] = false;
        } else {
            std::vector<QStringList> _rows = csv_datas.toVector().toStdVector();
            success[i] = ada.addRows(_rows, i);
        }
    }
    for (int i = 0; i < APAI_STATIC_TABLE_COUNTS; i++) {
        if (!success[i]) {
            return false;
        }
    }
    return true;
}

int main(int argc, char *argv[]) {
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
    bool db_ok = generate_db();
    if (!db_ok) {
        qDebug() << "Data base generate not successful.";
        return -1;
    }
    return app.exec();
}
