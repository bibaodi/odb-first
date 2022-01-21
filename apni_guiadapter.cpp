#include "apni_guiadapter.h"
#include <QDebug>

GuiAdapter::GuiAdapter(QObject *parent) : QObject(parent) { qDebug() << "GuiAdapter init~"; }

int GuiAdapter::getUtpDatasById(int utp_id) {
    qDebug() << "GuiAdapter:" << utp_id;
    return 0;
}
