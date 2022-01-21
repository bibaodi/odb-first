#ifndef GUIADAPTER_H
#define GUIADAPTER_H

#include <QObject>

class APNI_DB_Adapter;
class APnIGuiAdapter : public QObject {
    Q_OBJECT
  public:
    explicit APnIGuiAdapter(QObject *parent = nullptr);
    Q_INVOKABLE int getUtpDatasById(int utp_id);

  private:
    bool connect2db();
    APNI_DB_Adapter *m_db;

  signals:
};

#endif // GUIADAPTER_H
