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
#include "apni_db.h"
#include "apni_dbus_client.h"
#include "apni_guiadapter.h"
#include "documenthandler.h"
#include "jsoncpp/json/json.h"
#include <QDBusInterface>
#include <QDBusReply>
#include <QFile>
#include <vector>

#define APnI_STATIC_TABLE_COUNTS 4
#define APnI_DYNAMIC_TABLE_COUNTS 3 + 1

#define DEBUG_UI 0

bool generate_db() {
    bool success[APnI_STATIC_TABLE_COUNTS + APnI_DYNAMIC_TABLE_COUNTS] = {false};
    // APNI_DB_Adapter ada("apni-gen.db");
    APNI_DB_Adapter *ada = APNI_DB_Adapter::get_instance();
    if (ada->init_db(APnI_DB_VERSION) < 1) {
        qDebug() << "already created.";
        return true;
    }
    ada->addVersion("init by eton when testing, generate all db with datas.");

    QString csv_files[APnI_STATIC_TABLE_COUNTS + APnI_DYNAMIC_TABLE_COUNTS] = {
        QStringLiteral("probes"), QStringLiteral("modes"), QStringLiteral("apodizations"), QStringLiteral("pulses"),
        QLatin1String("null"),    QStringLiteral("UTPs"),  QStringLiteral("MTPs"),         QStringLiteral("UTP_Infos")};

    for (int i = 0; i < APnI_STATIC_TABLE_COUNTS + APnI_DYNAMIC_TABLE_COUNTS; i++) {
        QFile csvfile(QString("/tmp/%1.csv").arg(csv_files[i]));
        if (!csvfile.exists()) {
            qDebug() << "file Not exist. please ensure all exist~" << csvfile.fileName();
        }
    }
    for (int i = 0; i < APnI_STATIC_TABLE_COUNTS + APnI_DYNAMIC_TABLE_COUNTS; i++) {
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
            std::vector<QStringList> _rows =
                std::vector<QStringList>(csv_datas.toVector().begin(), csv_datas.toVector().end());
            success[i] = ada->addRows(_rows, i);
        }
        qDebug() << i << ": success=" << success[i];
    }
    for (int i = 0; i < APnI_STATIC_TABLE_COUNTS + APnI_DYNAMIC_TABLE_COUNTS; i++) {
        if (!success[i]) {
            return false;
        }
    }
    return true;
}

bool connect_to_dbus() {
#define DBUS_APNI_IFACE_NAME "com.esi.APnI"
#define DBUS_APNI_SERVICE_NAME "com.esi.APnI"
#define DBUS_APNI_OBJ_NAME "/apni"
    if (!QDBusConnection::sessionBus().isConnected()) {
        fprintf(stderr, "Cannot connect to the D-Bus session bus.\n"
                        "To start it, run:\n"
                        "\teval `dbus-launch --auto-syntax`\n");
        return false;
    }
    QDBusInterface *iface;
    iface = new QDBusInterface(DBUS_APNI_SERVICE_NAME, DBUS_APNI_OBJ_NAME, DBUS_APNI_IFACE_NAME,
                               QDBusConnection::sessionBus());
    if (!iface->isValid()) {
        fprintf(stderr, "%s\n", qPrintable(QDBusConnection::sessionBus().lastError().message()));
        QCoreApplication::instance()->quit();
    }
    // QDBusMessage reply = iface->call("key_ev_slot", "a", "p");
    QDBusReply<int> reply = iface->call("key_ev_slot", "a", "p");
    if (reply.isValid()) {
        qDebug() << "dbus reply=" << reply.value();
        return true;
    } else {
        qDebug() << "DBus call failed!" << reply.error().message();
        return false;
    }
}

int main(int argc, char *argv[]) {
    qmlRegisterType<APnIGuiAdapter>("EsiModule", 1, 0, "APnI_GuiAdapter");
    qmlRegisterType<APnI_DBUS_Client>("EsiModule", 1, 0, "APnI_DBUS_Client");
    qmlRegisterType<DocumentHandler>("EsiModule", 1, 0, "DocumentHandler");
    // qmlRegisterType<UTPs>("EsiModule", 1, 0, "UTPs");
    // qRegisterMetaType<UTPs>("UTPs");
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
    /*    bool dbus_ok = connect_to_dbus();
        if (!dbus_ok) {
            qDebug() << "connect to dbus error~";
            return -1;
        }
        */
    // toJason();
    return app.exec();
}
