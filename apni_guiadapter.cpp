#include "apni_guiadapter.h"
#include "apni_db_adapter.h"
#include <odb/database.hxx>
#include <odb/transaction.hxx>

#include "apni_db-odb.hxx"
#include "apni_db.h"
#include <QDebug>
#include <QQuickItem>

using namespace odb::core;

APnIGuiAdapter::APnIGuiAdapter(QObject *parent) : QObject(parent), m_db(nullptr), m_win(nullptr) {
    qDebug() << "APnIGuiAdapter init~";
    connect2db();
}

void APnIGuiAdapter::setRoot_win(QQuickWindow *obj) { m_win = obj; }

bool APnIGuiAdapter::update_utp_data(UTPs &utp) {
    if (nullptr == m_win) {
        qDebug() << "window not fount";
        return false;
    }
    QObject *qitem;
    qitem = m_win->findChild<QObject *>("ProbeName");
    if (qitem) {
        qitem->setProperty("text", APNI_DB_Adapter::ProbesNames[utp.id_probe]);
    }
    //--loc, nb-ele, apod--//
    qitem = m_win->findChild<QObject *>("Location");
    if (qitem) {
        qitem->setProperty("text", utp.location);
    }
    qitem = m_win->findChild<QObject *>("NB_ele");
    if (qitem) {
        qitem->setProperty("text", utp.nb_element);
    }
    qitem = m_win->findChild<QObject *>("id_Apod");
    if (qitem) {
        qitem->setProperty("text", utp.id_apodization);
    }
    //--freq, nb-half-cycle, polarity--//
    qitem = m_win->findChild<QObject *>("freq");
    if (qitem) {
        qitem->setProperty("text", utp.freq);
    }
    qitem = m_win->findChild<QObject *>("Nb_half_cycle");
    if (qitem) {
        qitem->setProperty("text", utp.nb_half_cycle);
    }
    qitem = m_win->findChild<QObject *>("polarity");
    if (qitem) {
        qitem->setProperty("text", utp.polarity);
    }
    //--mode, duty-cycle, pulse_type//
    qitem = m_win->findChild<QObject *>("id_Mode");
    if (qitem) {
        qitem->setProperty("text", utp.id_mode);
    }
    qitem = m_win->findChild<QObject *>("duty_cycle");
    if (qitem) {
        qitem->setProperty("text", utp.duty_cycle);
    }
    qitem = m_win->findChild<QObject *>("id_pulse_type");
    if (qitem) {
        qitem->setProperty("text", utp.id_pulse_type);
    }
    //--Voltage, PRF, startLine, stopLine--//
    qitem = m_win->findChild<QObject *>("Voltage");
    if (qitem) {
        qitem->setProperty("text", utp.voltage);
    }
    qitem = m_win->findChild<QObject *>("PRF");
    if (qitem) {
        qitem->setProperty("text", 0);
    }
    qitem = m_win->findChild<QObject *>("startLine");
    if (qitem) {
        qitem->setProperty("text", 1);
    }
    qitem = m_win->findChild<QObject *>("stopLine");
    if (qitem) {
        qitem->setProperty("text", 1);
    }
    //------/
    return true;
}

int APnIGuiAdapter::getUtpDatasById(int utp_id) {
    qDebug() << "APnIGuiAdapter: getUtpDatasById" << utp_id;
    // odb::query<Probes> _query(query::id_probe == utp_id);
    UTPs utpobj(false);
    int count = 0;
    if (m_db && m_db->isAvailable()) {
        count = m_db->lookupUtps(utp_id, utpobj);
        update_utp_data(utpobj);
    }
    return count;
}

bool APnIGuiAdapter::connect2db() {
    m_db = APNI_DB_Adapter::get_instance();
    if (m_db->isAvailable()) {
        return true;
    }
    return false;
}
