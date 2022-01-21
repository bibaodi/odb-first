#include "apni_guiadapter.h"
#include "apni_db_adapter.h"
#include <odb/database.hxx>
#include <odb/transaction.hxx>

#include <QDebug>

APnIGuiAdapter::APnIGuiAdapter(QObject *parent) : QObject(parent), m_db(nullptr) { qDebug() << "APnIGuiAdapter init~"; }

int APnIGuiAdapter::getUtpDatasById(int utp_id) {
    qDebug() << "APnIGuiAdapter: getUtpDatasById" << utp_id;
    // odb::query<UTPs> utp(query::id_utp == utp_id);
    odb::result<UTPs> utp_rets(m_db->query<UTPs>(odb::query<UTPs>::id_utp == utp_id));
    int n = utp_rets.size();
    if (n > 0) {
        foreach (UTPs &item, utp_rets) { qDebug() << item.id; }
    }
    return 0;
}

bool APnIGuiAdapter::connect2db() {
    m_db = APNI_DB_Adapter::get_instance();
    if (m_db->m_available) {
        return true;
    }
    return false;
}
