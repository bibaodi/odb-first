#include "apni_dbus_client.h"
#include <QDebug>

#define DBUS_APNI_IFACE_NAME "com.esi.APnI"
#define DBUS_APNI_SERVICE_NAME "com.esi.APnI"
#define DBUS_APNI_OBJ_NAME "/apni"

APnI_DBUS_Client::APnI_DBUS_Client(QObject *parent) : QObject(parent), m_available(false), m_iface(nullptr) {
    qDebug() << "APnI_DBUS_Client init~";
    connect_to_dbus();
}

bool APnI_DBUS_Client::connect_to_dbus() {
    if (!QDBusConnection::sessionBus().isConnected()) {
        fprintf(stderr, "Cannot connect to the D-Bus session bus.\n"
                        "To start it, run:\n"
                        "\teval `dbus-launch --auto-syntax`\n");
        return false;
    }

    m_iface = new QDBusInterface(DBUS_APNI_SERVICE_NAME, DBUS_APNI_OBJ_NAME, DBUS_APNI_IFACE_NAME,
                                 QDBusConnection::sessionBus());
    if (!m_iface->isValid()) {
        fprintf(stderr, "%s\n", qPrintable(QDBusConnection::sessionBus().lastError().message()));
        return false;
    }
    m_available = true;
    return true;
}

int APnI_DBUS_Client::run_cmd(QString param, QString cmd) {
    if (!m_available) {
        if (!connect_to_dbus()) {
            return -99;
        }
    }
    QDBusReply<int> reply = m_iface->call("key_ev_slot", cmd, param);
    if (reply.isValid()) {
        qDebug() << "dbus reply=" << reply.value();
        return 0;
    } else {
        qDebug() << "DBus call failed!" << reply.error().message();
        return -1;
    }
}
