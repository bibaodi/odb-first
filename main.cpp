// file      : hello/driver.cxx
// copyright : not copyrighted - public domain

#include <iostream>
#include <memory> // std::shared_ptr

#include <odb/database.hxx>
#include <odb/transaction.hxx>

#include "database_create.h" // create_database

#include "apni_db_adapter.h"
//#include "person-odb.hxx"
#include "person.h"

#include <QGuiApplication>
#include <QQmlApplicationEngine>

//--csv.begin
#include "reader.h"
#include "stringdata.h"
#include "writer.h"
//--csv.begin.end
#include <QFile>
#include <vector>

#define APAI_STATIC_TABLE_COUNTS 4
#define APAI_DYNAMIC_TABLE_COUNTS 3 + 1

#define DEBUG_UI 0

bool generate_db() {
    bool success[APAI_STATIC_TABLE_COUNTS + APAI_DYNAMIC_TABLE_COUNTS] = {false};
    APNI_DB_Adapter ada("apni-gen.db");
    ada.addVersion("init by eton when testing, generate all db with datas.");

    QString csv_files[APAI_STATIC_TABLE_COUNTS + APAI_DYNAMIC_TABLE_COUNTS] = {
        QStringLiteral("probes"), QStringLiteral("modes"), QStringLiteral("apodizations"), QStringLiteral("pulses"),
        QLatin1String("null"),    QStringLiteral("UTPs"),  QStringLiteral("MTPs"),         QStringLiteral("UTP_Infos")};

    for (int i = 0; i < APAI_STATIC_TABLE_COUNTS + APAI_DYNAMIC_TABLE_COUNTS; i++) {
        QFile csvfile(QString("/tmp/%1.csv").arg(csv_files[i]));
        if (!csvfile.exists()) {
            qDebug() << "file Not exist. please ensure all exist~" << csvfile.fileName();
        }
    }
    for (int i = 0; i < APAI_STATIC_TABLE_COUNTS + APAI_DYNAMIC_TABLE_COUNTS; i++) {
        if (csv_files[i].contains("null", Qt::CaseInsensitive)) {
            success[i] = true;
            continue;
        }
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
        qDebug() << i << ": success=" << success[i];
    }
    for (int i = 0; i < APAI_STATIC_TABLE_COUNTS + APAI_DYNAMIC_TABLE_COUNTS; i++) {
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
    if (!DEBUG_UI) {
        //        int n = 0;
        //        char *argv0[1] = {nullptr};
        //        create_database(n, argv0);
        bool db_ok = generate_db();
        if (!db_ok) {
            qDebug() << "Data base generate not successful.";
            return -1;
        } else {
            qDebug() << "Data base generate successful... congratulations~~";
        }
    }
    return app.exec();
}
