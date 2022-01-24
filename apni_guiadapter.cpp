#include "apni_guiadapter.h"
#include "apni_db_adapter.h"
#include <odb/database.hxx>
#include <odb/transaction.hxx>

#include "apni_db-odb.hxx"
#include "apni_db.h"
#include <QDebug>

using namespace odb::core;

APnIGuiAdapter::APnIGuiAdapter(QObject *parent) : QObject(parent), m_db(nullptr) {
    qDebug() << "APnIGuiAdapter init~";
    connect2db();
}

int APnIGuiAdapter::getUtpDatasById(int utp_id) {
    qDebug() << "APnIGuiAdapter: getUtpDatasById" << utp_id;
    // odb::query<Probes> _query(query::id_probe == utp_id);
    if (m_db && m_db->isAvailable()) {
        m_db->lookupUtps(utp_id);
    }
    return 0;
}

bool APnIGuiAdapter::connect2db() {
    m_db = APNI_DB_Adapter::get_instance();
    if (m_db->isAvailable()) {
        return true;
    }
    return false;
}
