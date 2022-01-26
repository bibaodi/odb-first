#ifndef APNI_DBUS_CLIENT_H
#define APNI_DBUS_CLIENT_H

#include <QDBusInterface>
#include <QDBusReply>
#include <QObject>

class APnI_DBUS_Client : public QObject {
    Q_OBJECT
  public:
    explicit APnI_DBUS_Client(QObject *parent = nullptr);
    bool connect_to_dbus();
    Q_INVOKABLE int run_cmd(QString param, QString cmdType);

  private:
    bool m_available;
    QDBusInterface *m_iface;
  signals:
};

#endif // APNI_DBUS_CLIENT_H
