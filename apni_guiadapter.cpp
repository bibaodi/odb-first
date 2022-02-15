#include "apni_guiadapter.h"
#include "apni_db_adapter.h"
#include <odb/database.hxx>
#include <odb/transaction.hxx>

#include "apni_db-odb.hxx"
#include "apni_db.h"
#include "jsoncpp/json/json.h"
#include <QDebug>
#include <QQuickItem>
using namespace odb::core;

const QString APnIGuiAdapter::UTPkeys[] = {"ProbeName",     "Location", "NB_ele",    "id_apod",    "freq",
                                           "Nb_half_cycle", "polarity", "id_mode",   "duty_cycle", "id_pulse_type",
                                           "voltage",       "PRF",      "startLine", "stopLine"};

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
        if (utp.id_probe < APNI_DB_Adapter::PROBENAMELEN) {
            qitem->setProperty("text", APNI_DB_Adapter::ProbesNames[utp.id_probe]);
        }
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
        qitem->setProperty("text", utp.transmit_lines);
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
        if (count) {
            update_utp_data(utpobj);
        } else {
            qDebug() << "No Utp Found from db using id=" << utp_id;
        }
    }
    return count;
}

int APnIGuiAdapter::setValueByIndex(int indx, float _val, UTPs *utp) {
    float prf = -1, startline = -1, stopline = -1;
    switch (indx) {
    case 1:
        utp->location = _val;
        break;
    case 2:
        utp->nb_element = _val;
        break;
    case 3:
        utp->id_apodization = _val;
        break;
    case 4:
        utp->freq = _val;
        break;
    case 5:
        utp->nb_half_cycle = _val;
        break;
    case 6:
        utp->polarity = _val;
        break;
    case 7:
        utp->id_mode = _val;
        break;
    case 8:
        utp->duty_cycle = _val;
        break;
    case 9:
        utp->id_pulse_type = _val;
        break;
    case 10:
        utp->voltage = _val;
        break;
    case 11:
        prf = _val;
        break;
    case 12:
        startline = _val;
        return 12;
    case 13:
        stopline = _val;
        return 13;
    default:
        break;
    }
    return 0;
}

int APnIGuiAdapter::storeUtpItem2DB(QString utp_json) {
    const int json_str_min_len = 10;
    if (utp_json.length() < json_str_min_len) {
        return -1;
    }
    const std::string rawJson = utp_json.toStdString();
    const auto rawJsonLength = static_cast<int>(rawJson.length());

    JSONCPP_STRING err;
    Json::Value root;
    Json::CharReaderBuilder builder;
    const std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
    if (!reader->parse(rawJson.c_str(), rawJson.c_str() + rawJsonLength, &root, &err)) {
        qDebug() << "json parse error!";
        return -2;
    }
    const int utp_key_count = 14;

    UTPs utp(true);
    int start_line = 0, stop_line = 0, ret = -1;
    for (int i = 1; i < utp_key_count; i++) {
        const QString &keystr = UTPkeys[i];
        float json_value = root.get(keystr.toStdString(), "-1").asFloat();
        // qDebug() << "i=" << i << "], value=" << json_value;
        ret = setValueByIndex(i, json_value, &utp);
        switch (ret) {
        case 12:
            start_line = json_value;
            break;
        case 13:
            stop_line = json_value;
            break;
        default:
            break;
        }
    }
    utp.transmit_lines = stop_line - start_line + 1;
    APNI_DB_Adapter *ada = APNI_DB_Adapter::get_instance();
    bool db_result = true;
    if (ada) {
        // TODO: return utp_id
        db_result = ada->addUtpRow(utp);
        if (!db_result) {
            qDebug() << "add utp item to db error";
            return -1;
        }
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
