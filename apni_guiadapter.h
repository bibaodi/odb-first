#ifndef GUIADAPTER_H
#define GUIADAPTER_H

#include <QObject>

class GuiAdapter : public QObject {
    Q_OBJECT
  public:
    explicit GuiAdapter(QObject *parent = nullptr);
    Q_INVOKABLE int getUtpDatasById(int utp_id);

  private:
    bool connect2db();

  signals:
};

#endif // GUIADAPTER_H
