#ifndef GUIADAPTER_H
#define GUIADAPTER_H

#include <QObject>
#include <QQuickWindow>

class APNI_DB_Adapter;
class UTPs;
class APnIGuiAdapter : public QObject {
    Q_OBJECT
  public:
    explicit APnIGuiAdapter(QObject *parent = nullptr);
    Q_INVOKABLE int getUtpDatasById(int utp_id);
    Q_INVOKABLE void setRoot_win(QQuickWindow *);
    bool update_utp_data(UTPs &);

  private:
    bool connect2db();
    APNI_DB_Adapter *m_db;
    QQuickWindow *m_win;

  signals:
};

#endif // GUIADAPTER_H
