#include "apni_db_adapter.h"
#include "apni_db-odb.hxx"
#include "apni_db.h"
#include <QTextCodec>
#include <iostream>
#include <odb/database.hxx>
#include <odb/sqlite/exceptions.hxx>
#include <odb/transaction.hxx>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>

using namespace odb::core;

const QString APNI_DB_Adapter::TableNames[] = {
    QStringLiteral("Probes"),     QStringLiteral("Modes"),    QStringLiteral("Apodizations"),
    QStringLiteral("PulseTypes"), QStringLiteral("null"),     QStringLiteral("UTPs"),
    QStringLiteral("MTPs"),       QStringLiteral("UTPInfos"), QLatin1Literal()};

const QString APNI_DB_Adapter::UTP_ColNames[] = {
    "id_utp",         " id_pulse_type", "id_probe",      "id_mode",  "id_apodization", "id_mtp",        "location",
    "nb_element",     "freq",           "nb_half_cycle", "polarity", "transmit_lines", "composed_mode", "duty_cycle",
    "nb_element_max", "voltage",        "delta",         "deltaT",   "manipulated",
};

const QString APNI_DB_Adapter::UTPINFO_ColNames[] = {
    "id_utp",
    "framerate_multiplier",
    "framerate_multiplier_m",
    "max_voltage",
    "voltage",
    "mi",
    "tib",
    "tis",
    "ispta",
    "valid",
    "presets",
    "tic",
    "settings",
};

const QString APNI_DB_Adapter::MTP_ColNames[] = {
    "id_mtp",       "id_utp",         "T",        "mi",          "mi_inv",      "pii_0_u",
    "pii_3_u",      "pii_0_s",        "pii_3_s",  "pii_3_inv_u", "pii_3_inv_s", "w0",
    "tisas_u",      "tisas_s_factor", "tisbs_u",  "tibbs_u",     "ticas_u",     "ticas_s_factor",
    "td_u",         "td_s",           "ec_hv",    "pd",          "pr",          "pc",
    "isppa",        "ispta",          "fc3",      "fc6",         "pr3",         "tisbs_s",
    "tibbs_s",      "w0_s",           "z0",       "z3",          "sigma",       "delta",
    "ispta_s",      "ipa3_mi",        "ispta3_s", "FL_Azim",     "FL_Elev",     "AaptAzim",
    "deq4MI",       "deq4TIB",        "zB3",      "w01_mW",      "zBP_cm",      "z1_cm",
    "minW3ITA3_mW", "ticas_s",        "tisas_s",
};

APNI_DB_Adapter *APNI_DB_Adapter::get_instance() {
    static APNI_DB_Adapter obj("apni-gen.db");
    return &obj;
}

APNI_DB_Adapter::APNI_DB_Adapter(const QString &file_name) : ESIDatabase(file_name, DB_SCHEMA_NAME, APnI_DB_VERSION) {}

bool APNI_DB_Adapter::isAvailable() { return m_available; }

bool APNI_DB_Adapter::isHeaderMatch(const QStringList &csvHeaders, int table_type) {
    qDebug() << "\nisHeaderMatch >>" << TableNames[table_type];
    if (csvHeaders.length() < 2) {
        return false;
    }
    bool contain_id = false;
    foreach (const QString &csvHeader, csvHeaders) {
        if (csvHeader.contains("id_")) {
            contain_id = true;
        }
    }
    if (!contain_id) {
        qDebug() << "isHeaderMatch: no id_ found!" << csvHeaders;
        return false;
    }

    bool is_ok = false;
    QList<TableInfo> tbinfos;
    is_ok = queryTableInfo(TableNames[table_type], tbinfos);
    if (!is_ok) {
        qDebug() << "sql run error.";
    } else {
        for (int i = 0; i < tbinfos.length(); i++)
            qDebug() << "table infos:" << tbinfos[i].name << tbinfos[i].type;
    }
    const int db_cols = tbinfos.length();
    if (csvHeaders.length() > db_cols) {
        qDebug() << "too many colums for current DB_Table" << TableNames[table_type];
        return false;
    }

    foreach (const QString &header, csvHeaders) {
        int i = 0;
        for (i = 0; i < db_cols; i++) {
            if (!QString::compare(header, tbinfos[i].name, Qt::CaseInsensitive)) {
                break;
            }
        }
        if (i == db_cols) {
            return false;
        }
    }

    return true;
}

bool APNI_DB_Adapter::queryTableInfo(const QString &tb_name, QList<TableInfo> &tbinfos) {
    try {
        char **result = nullptr;
        int cols;
        int rows;
        QString sql_str = "PRAGMA table_info(" + tb_name + ");";
        QByteArray str = get_codec()->fromUnicode(sql_str);
        int rc;
        rc = sqlite3_get_table(get_handler(), str.data(), &result, &rows, &cols, nullptr);
        QString strOut;
        int nIndex = cols;
        // 0.cid [0,1...]; 1.name [str]; 2.type [str]; 3.notnull[0/1]; 4. dflt_value; 5.pk[]0/1;
        for (int i = 0; i < rows; i++) {
            TableInfo tinfo;
            tinfo.cid = QString(result[(i + 1) * cols + static_cast<int>(SQLITE_TABLE_INFO_IDX::CID)]).toInt();
            tinfo.name = QString(result[(i + 1) * cols + static_cast<int>(SQLITE_TABLE_INFO_IDX::NAME)]);
            tinfo.type = QString(result[(i + 1) * cols + static_cast<int>(SQLITE_TABLE_INFO_IDX::TYPE)]);
            tinfo.dflt_value = QString(result[(i + 1) * cols + static_cast<int>(SQLITE_TABLE_INFO_IDX::DFLT_VALUE)]);
            tinfo.notnull = QString(result[(i + 1) * cols + static_cast<int>(SQLITE_TABLE_INFO_IDX::NOTNULL)]).toInt();
            tinfo.pk = QString(result[(i + 1) * cols + static_cast<int>(SQLITE_TABLE_INFO_IDX::PK)]).toInt();
            tbinfos.append(tinfo);
        }
        sqlite3_free_table(result);
        return (rows > 1);
    } catch (...) {
        return false;
    }
}

void APNI_DB_Adapter::addVersion(const QString &msg) {
    // transaction tr(beginTrans());
    beginTrans();
    Versions _version(msg);
    persist<Versions>(_version);
    commitTrans();
}

bool APNI_DB_Adapter::addProbe(const int &_id, const QString &_name) {
    bool ret_ok = false;
    ret_ok = beginTrans();
    if (!ret_ok) {
        return ret_ok;
    }
    Probes _probe(_id, _name);
    try {
        persist<Probes>(_probe);
    } catch (const odb::exception &e) {
        qDebug() << e.what();
        return false;
    }

    ret_ok = commitTrans();

    return ret_ok;
}

bool APNI_DB_Adapter::addProbes(const vector<int> &_ids, const vector<QString> &_names) {
    bool ret_ok = false;
    const int len_id = _ids.size();
    const int len_name = _names.size();
    if (len_id != len_name) {
        return ret_ok;
    }
    for (int i = 0; i < len_id; i++) {
        ret_ok = beginTrans();
        if (!ret_ok) {
            return ret_ok;
        }
        Probes _probe(_ids[i], _names[i]);
        try {
            persist<Probes>(_probe);
        } catch (const odb::exception &e) {
            qDebug() << e.what();
            continue;
        }
        ret_ok = commitTrans();
        if (!ret_ok) {
            return ret_ok;
        }
    }

    return ret_ok;
}

bool APNI_DB_Adapter::addMode(const int &_id, const QString &_name) {
    bool ret_ok = false;
    ret_ok = beginTrans();
    if (!ret_ok) {
        return ret_ok;
    }

    Modes _probe(_id, _name);
    try {
        persist<Modes>(_probe);
    } catch (const odb::exception &e) {
        qDebug() << e.what();
        return false;
    }

    ret_ok = commitTrans();

    return ret_ok;
}

bool APNI_DB_Adapter::addModes(const vector<int> &_ids, const vector<QString> &_names) {
    bool ret_ok = false;
    const int len_id = _ids.size();
    const int len_name = _names.size();
    if (len_id != len_name) {
        return ret_ok;
    }
    for (int i = 0; i < len_id; i++) {
        ret_ok = beginTrans();
        if (!ret_ok) {
            return ret_ok;
        }
        Modes _probe(_ids[i], _names[i]);
        try {
            persist<Modes>(_probe);
        } catch (const odb::exception &e) {
            qDebug() << e.what();
            continue;
        }
        ret_ok = commitTrans();
        if (!ret_ok) {
            return ret_ok;
        }
    }

    return ret_ok;
}

bool APNI_DB_Adapter::addRow(void *object_item, int table_type) {
    bool ret_ok = false;
    ret_ok = beginTrans();
    if (!ret_ok) {
        return ret_ok;
    }

    switch (table_type) {
    case TABLE_UTP: {
        try {
            UTPs *ptr = (UTPs *)object_item;
            persist<UTPs>(*ptr);
        } catch (const odb::exception &e) {
            qDebug() << e.what();
            return false;
        }
        break;
    }
    case TABLE_MTP: {
        try {
            MTPs *ptr = (MTPs *)object_item;
            persist<MTPs>(*ptr);
        } catch (const odb::exception &e) {
            qDebug() << e.what();
            return false;
        }
        break;
    }
    case TABLE_UTPI: {
        try {
            UTPInfos *ptr = (UTPInfos *)object_item;
            persist<UTPInfos>(*ptr);
        } catch (const odb::exception &e) {
            qDebug() << e.what();
            return false;
        }
        break;
    }
    }
    ret_ok = commitTrans();

    return ret_ok;
}

inline bool APNI_DB_Adapter::addUtpRow(const UTPs &utp) { return addRow((void *)(&utp), static_cast<int>(TABLE_UTP)); }

inline bool APNI_DB_Adapter::addMtpRow(const MTPs &mtp) { return addRow((void *)(&mtp), static_cast<int>(TABLE_MTP)); }

inline bool APNI_DB_Adapter::addUtpiRow(const UTPInfos &utpinfo) {
    return addRow((void *)(&utpinfo), static_cast<int>(TABLE_UTPI));
}

bool APNI_DB_Adapter::addRow(const int &_id, const QString &_name, int table_type) {
    bool ret_ok = false;
    ret_ok = beginTrans();
    if (!ret_ok) {
        return ret_ok;
    }
    switch (table_type) {
    case TABLE_MODE: {
        Modes _probe(_id, _name);
        try {
            persist<Modes>(_probe);
        } catch (const odb::exception &e) {
            qDebug() << e.what();
            return false;
        }
        break;
    }
    case TABLE_PROBE: {
        Modes _probe(_id, _name);
        try {
            persist<Modes>(_probe);
        } catch (const odb::exception &e) {
            qDebug() << e.what();
            return false;
        }
        break;
    }
    case TABLE_PULSE: {
        PulseTypes _pulse(_id, _name);
        try {
            persist<PulseTypes>(_pulse);
        } catch (const odb::exception &e) {
            qDebug() << e.what();
            return false;
        }
        break;
    }
    case TABLE_APOD: {
        Apodizations _apod(_id, _name);
        try {
            persist<Apodizations>(_apod);
        } catch (const odb::exception &e) {
            qDebug() << e.what();
            return false;
        }
        break;
    }
    default:
        break;
    }

    ret_ok = commitTrans();

    return ret_ok;
}

bool APNI_DB_Adapter::addRows(const vector<QStringList> &_rows, int table_type) {
    QStringList _row = _rows[0];
    bool ret_ok = false;
    int err_count = 0;

    ret_ok = isHeaderMatch(_row, table_type);
    if (!ret_ok) {
        qDebug() << "Header not match~!" << TableNames[table_type];
        return false;
    }
    const QStringList headers = _rows[0];

    for (int _ri = 1; _ri < _rows.size(); _ri++) {
        if (_ri < 10) {
            qDebug() << "row=" << _rows[_ri] << "::" << _ri;
        }
        _row = _rows[_ri];
        if (_row.length() < 2) {
            return false;
        }
        if (_row.contains("id_")) {
            qDebug() << "CSV Table Header:" << _row;
            continue;
        }
        switch (table_type) {
        case TABLE_MODE:
            ret_ok = addMode(_row[0].toInt(), _row[1]);
            break;
        case TABLE_PROBE:
            ret_ok = addProbe(_row[0].toInt(), _row[1]);
            break;
        case TABLE_PULSE:
        case TABLE_APOD: {
            ret_ok = addRow(_row[0].toInt(), _row[1], table_type);
            break;
        }
        case TABLE_UTP: {
            UTPs utp(false);
            for (int _ic = 0; _ic < headers.length(); _ic++) {
                QString item = _row[_ic];
                if (!QString::compare(headers[_ic], UTP_ColNames[static_cast<int>(UTP_COLs::id_utp)],
                                      Qt::CaseInsensitive)) {
                    utp.id_utp = item.toInt();
                }
                if (!QString::compare(headers[_ic], UTP_ColNames[static_cast<int>(UTP_COLs::id_pulse_type)],
                                      Qt::CaseInsensitive)) {
                    utp.id_pulse_type = item.toInt();
                }
                if (!QString::compare(headers[_ic], UTP_ColNames[static_cast<int>(UTP_COLs::id_probe)],
                                      Qt::CaseInsensitive)) {
                    utp.id_probe = item.toInt();
                }
                if (!QString::compare(headers[_ic], UTP_ColNames[static_cast<int>(UTP_COLs::id_mode)],
                                      Qt::CaseInsensitive)) {
                    utp.id_mode = item.toInt();
                }
                if (!QString::compare(headers[_ic], UTP_ColNames[static_cast<int>(UTP_COLs::id_apodization)],
                                      Qt::CaseInsensitive)) {
                    utp.id_apodization = item.toInt();
                }
                if (!QString::compare(headers[_ic], UTP_ColNames[static_cast<int>(UTP_COLs::id_mtp)],
                                      Qt::CaseInsensitive)) {
                    utp.id_mtp = item.toInt();
                }
                if (!QString::compare(headers[_ic], UTP_ColNames[static_cast<int>(UTP_COLs::location)],
                                      Qt::CaseInsensitive)) {
                    utp.location = item.toInt();
                }
                if (!QString::compare(headers[_ic], UTP_ColNames[static_cast<int>(UTP_COLs::nb_element)],
                                      Qt::CaseInsensitive)) {
                    utp.nb_element = item.toInt();
                }
                if (!QString::compare(headers[_ic], UTP_ColNames[static_cast<int>(UTP_COLs::freq)],
                                      Qt::CaseInsensitive)) {
                    utp.freq = item.toDouble();
                }
                if (!QString::compare(headers[_ic], UTP_ColNames[static_cast<int>(UTP_COLs::nb_half_cycle)],
                                      Qt::CaseInsensitive)) {
                    utp.nb_half_cycle = item.toInt();
                }
                if (!QString::compare(headers[_ic], UTP_ColNames[static_cast<int>(UTP_COLs::polarity)],
                                      Qt::CaseInsensitive)) {
                    utp.polarity = item.toInt();
                }
                if (!QString::compare(headers[_ic], UTP_ColNames[static_cast<int>(UTP_COLs::transmit_lines)],
                                      Qt::CaseInsensitive)) {
                    utp.transmit_lines = item.toInt();
                }
                if (!QString::compare(headers[_ic], UTP_ColNames[static_cast<int>(UTP_COLs::composed_mode)],
                                      Qt::CaseInsensitive)) {
                    utp.composed_mode = item.toInt();
                }
                if (!QString::compare(headers[_ic], UTP_ColNames[static_cast<int>(UTP_COLs::duty_cycle)],
                                      Qt::CaseInsensitive)) {
                    utp.duty_cycle = item.toInt();
                }
                if (!QString::compare(headers[_ic], UTP_ColNames[static_cast<int>(UTP_COLs::nb_element_max)],
                                      Qt::CaseInsensitive)) {
                    utp.nb_element_max = item.toInt();
                }
                if (!QString::compare(headers[_ic], UTP_ColNames[static_cast<int>(UTP_COLs::voltage)],
                                      Qt::CaseInsensitive)) {
                    utp.voltage = item.toDouble();
                }
                if (!QString::compare(headers[_ic], UTP_ColNames[static_cast<int>(UTP_COLs::delta)],
                                      Qt::CaseInsensitive)) {
                    utp.delta = item.toInt();
                }
                if (!QString::compare(headers[_ic], UTP_ColNames[static_cast<int>(UTP_COLs::deltaT)],
                                      Qt::CaseInsensitive)) {
                    utp.deltaT = item.toDouble();
                }
                if (!QString::compare(headers[_ic], UTP_ColNames[static_cast<int>(UTP_COLs::manipulated)],
                                      Qt::CaseInsensitive)) {
                    utp.manipulated = item.toInt();
                }
                if (_ri > 100) {
                    return true;
                }
            }
            ret_ok = addUtpRow(utp);
            if (!ret_ok && err_count < 10) {
                qDebug() << _ri << ":]utp not insert successful:" << _row;
                err_count++;
                ret_ok = true;
            }
            break;
        }
        case TABLE_UTPI: {
            UTPInfos utpinfo;
            for (int _ic = 0; _ic < headers.length(); _ic++) {
                QString *item = &(_row[_ic]);
                if (!QString::compare(headers[_ic], UTPINFO_ColNames[static_cast<int>(UTPINFO_COLs::id_utp)],
                                      Qt::CaseInsensitive)) {
                    utpinfo.id_utp = item->toInt();
                }
                if (!QString::compare(headers[_ic],
                                      UTPINFO_ColNames[static_cast<int>(UTPINFO_COLs::framerate_multiplier)],
                                      Qt::CaseInsensitive)) {
                    utpinfo.framerate_multiplier = item->toDouble();
                }
                if (!QString::compare(headers[_ic],
                                      UTPINFO_ColNames[static_cast<int>(UTPINFO_COLs::framerate_multiplier_m)],
                                      Qt::CaseInsensitive)) {
                    utpinfo.framerate_multiplier_m = item->toDouble();
                }
                if (!QString::compare(headers[_ic], UTPINFO_ColNames[static_cast<int>(UTPINFO_COLs::max_voltage)],
                                      Qt::CaseInsensitive)) {
                    utpinfo.max_voltage = item->toDouble();
                }
                if (!QString::compare(headers[_ic], UTPINFO_ColNames[static_cast<int>(UTPINFO_COLs::voltage)],
                                      Qt::CaseInsensitive)) {
                    utpinfo.voltage = item->toDouble();
                }
                if (!QString::compare(headers[_ic], UTPINFO_ColNames[static_cast<int>(UTPINFO_COLs::mi)],
                                      Qt::CaseInsensitive)) {
                    utpinfo.mi = item->toDouble();
                }
                if (!QString::compare(headers[_ic], UTPINFO_ColNames[static_cast<int>(UTPINFO_COLs::tib)],
                                      Qt::CaseInsensitive)) {
                    utpinfo.tib = item->toDouble();
                }
                if (!QString::compare(headers[_ic], UTPINFO_ColNames[static_cast<int>(UTPINFO_COLs::tis)],
                                      Qt::CaseInsensitive)) {
                    utpinfo.tis = item->toDouble();
                }
                if (!QString::compare(headers[_ic], UTPINFO_ColNames[static_cast<int>(UTPINFO_COLs::ispta)],
                                      Qt::CaseInsensitive)) {
                    utpinfo.ispta = item->toDouble();
                }
                if (!QString::compare(headers[_ic], UTPINFO_ColNames[static_cast<int>(UTPINFO_COLs::valid)],
                                      Qt::CaseInsensitive)) {
                    utpinfo.valid = item->toInt();
                }
                if (!QString::compare(headers[_ic], UTPINFO_ColNames[static_cast<int>(UTPINFO_COLs::mi)],
                                      Qt::CaseInsensitive)) {
                    utpinfo.presets = *item;
                }
                if (!QString::compare(headers[_ic], UTPINFO_ColNames[static_cast<int>(UTPINFO_COLs::tic)],
                                      Qt::CaseInsensitive)) {
                    utpinfo.tic = item->toDouble();
                }
                if (!QString::compare(headers[_ic], UTPINFO_ColNames[static_cast<int>(UTPINFO_COLs::settings)],
                                      Qt::CaseInsensitive)) {
                    utpinfo.settings = *item;
                }
            }
            ret_ok = addRow((void *)&utpinfo, table_type);
            if (!ret_ok && err_count < 10) {
                qDebug() << _ri << ":]utpInfo not insert successful:" << _row;
                err_count++;
                ret_ok = true;
            }
            break;
        }
        case TABLE_MTP: {
            MTPs mtp;
            for (int _ic = 0; _ic < headers.length(); _ic++) {
                QString *item = &(_row[_ic]);
                if (!QString::compare(headers[_ic], MTP_ColNames[static_cast<int>(MTP_COLs::id_mtp)],
                                      Qt::CaseInsensitive)) {
                    mtp.id_mtp = item->toInt();
                }
                if (!QString::compare(headers[_ic], MTP_ColNames[static_cast<int>(MTP_COLs::id_utp)],
                                      Qt::CaseInsensitive)) {
                    mtp.id_utp = item->toInt();
                }
                if (!QString::compare(headers[_ic], MTP_ColNames[static_cast<int>(MTP_COLs::T)], Qt::CaseInsensitive)) {
                    mtp.T = item->toInt();
                }
                if (!QString::compare(headers[_ic], MTP_ColNames[static_cast<int>(MTP_COLs::mi)],
                                      Qt::CaseInsensitive)) {
                    mtp.mi = item->toDouble();
                }
                if (!QString::compare(headers[_ic], MTP_ColNames[static_cast<int>(MTP_COLs::mi_inv)],
                                      Qt::CaseInsensitive)) {
                    mtp.mi_inv = item->toDouble();
                }
                if (!QString::compare(headers[_ic], MTP_ColNames[static_cast<int>(MTP_COLs::pii_0_u)],
                                      Qt::CaseInsensitive)) {
                    mtp.pii_0_u = item->toDouble();
                }
                if (!QString::compare(headers[_ic], MTP_ColNames[static_cast<int>(MTP_COLs::pii_3_u)],
                                      Qt::CaseInsensitive)) {
                    mtp.pii_3_u = item->toDouble();
                }
                if (!QString::compare(headers[_ic], MTP_ColNames[static_cast<int>(MTP_COLs::pii_0_s)],
                                      Qt::CaseInsensitive)) {
                    mtp.pii_0_s = item->toDouble();
                }
                if (!QString::compare(headers[_ic], MTP_ColNames[static_cast<int>(MTP_COLs::pii_3_s)],
                                      Qt::CaseInsensitive)) {
                    mtp.pii_3_s = item->toDouble();
                }
                if (!QString::compare(headers[_ic], MTP_ColNames[static_cast<int>(MTP_COLs::pii_3_inv_u)],
                                      Qt::CaseInsensitive)) {
                    mtp.pii_3_inv_u = item->toDouble();
                }
                if (!QString::compare(headers[_ic], MTP_ColNames[static_cast<int>(MTP_COLs::pii_3_inv_s)],
                                      Qt::CaseInsensitive)) {
                    mtp.pii_3_inv_s = item->toDouble();
                }
                if (!QString::compare(headers[_ic], MTP_ColNames[static_cast<int>(MTP_COLs::w0)],
                                      Qt::CaseInsensitive)) {
                    mtp.w0 = item->toDouble();
                }
                if (!QString::compare(headers[_ic], MTP_ColNames[static_cast<int>(MTP_COLs::tisas_u)],
                                      Qt::CaseInsensitive)) {
                    mtp.tisas_u = item->toDouble();
                }
                if (!QString::compare(headers[_ic], MTP_ColNames[static_cast<int>(MTP_COLs::tisas_s_factor)],
                                      Qt::CaseInsensitive)) {
                    mtp.tisas_s_factor = item->toDouble();
                }
                if (!QString::compare(headers[_ic], MTP_ColNames[static_cast<int>(MTP_COLs::tisbs_u)],
                                      Qt::CaseInsensitive)) {
                    mtp.tisbs_u = item->toDouble();
                }
                if (!QString::compare(headers[_ic], MTP_ColNames[static_cast<int>(MTP_COLs::tibbs_u)],
                                      Qt::CaseInsensitive)) {
                    mtp.tibbs_u = item->toDouble();
                }
                if (!QString::compare(headers[_ic], MTP_ColNames[static_cast<int>(MTP_COLs::ticas_u)],
                                      Qt::CaseInsensitive)) {
                    mtp.ticas_u = item->toDouble();
                }
                if (!QString::compare(headers[_ic], MTP_ColNames[static_cast<int>(MTP_COLs::ticas_s_factor)],
                                      Qt::CaseInsensitive)) {
                    mtp.ticas_s_factor = item->toDouble();
                }
                if (!QString::compare(headers[_ic], MTP_ColNames[static_cast<int>(MTP_COLs::td_u)],
                                      Qt::CaseInsensitive)) {
                    mtp.td_u = item->toDouble();
                }
                if (!QString::compare(headers[_ic], MTP_ColNames[static_cast<int>(MTP_COLs::td_s)],
                                      Qt::CaseInsensitive)) {
                    mtp.td_s = item->toDouble();
                }
                if (!QString::compare(headers[_ic], MTP_ColNames[static_cast<int>(MTP_COLs::ec_hv)],
                                      Qt::CaseInsensitive)) {
                    mtp.ec_hv = item->toDouble();
                }
                if (!QString::compare(headers[_ic], MTP_ColNames[static_cast<int>(MTP_COLs::pd)],
                                      Qt::CaseInsensitive)) {
                    mtp.pd = item->toDouble();
                }
                if (!QString::compare(headers[_ic], MTP_ColNames[static_cast<int>(MTP_COLs::pr)],
                                      Qt::CaseInsensitive)) {
                    mtp.pr = item->toDouble();
                }
                if (!QString::compare(headers[_ic], MTP_ColNames[static_cast<int>(MTP_COLs::pc)],
                                      Qt::CaseInsensitive)) {
                    mtp.pc = item->toDouble();
                }
                if (!QString::compare(headers[_ic], MTP_ColNames[static_cast<int>(MTP_COLs::isppa)],
                                      Qt::CaseInsensitive)) {
                    mtp.isppa = item->toDouble();
                }
                if (!QString::compare(headers[_ic], MTP_ColNames[static_cast<int>(MTP_COLs::ispta)],
                                      Qt::CaseInsensitive)) {
                    mtp.ispta = item->toDouble();
                }
                if (!QString::compare(headers[_ic], MTP_ColNames[static_cast<int>(MTP_COLs::fc3)],
                                      Qt::CaseInsensitive)) {
                    mtp.fc3 = item->toDouble();
                }
                if (!QString::compare(headers[_ic], MTP_ColNames[static_cast<int>(MTP_COLs::fc6)],
                                      Qt::CaseInsensitive)) {
                    mtp.fc6 = item->toDouble();
                }
                if (!QString::compare(headers[_ic], MTP_ColNames[static_cast<int>(MTP_COLs::pr3)],
                                      Qt::CaseInsensitive)) {
                    mtp.pr3 = item->toDouble();
                }
                if (!QString::compare(headers[_ic], MTP_ColNames[static_cast<int>(MTP_COLs::tisbs_s)],
                                      Qt::CaseInsensitive)) {
                    mtp.tisbs_s = item->toDouble();
                }
                if (!QString::compare(headers[_ic], MTP_ColNames[static_cast<int>(MTP_COLs::tibbs_s)],
                                      Qt::CaseInsensitive)) {
                    mtp.tibbs_s = item->toDouble();
                }
                if (!QString::compare(headers[_ic], MTP_ColNames[static_cast<int>(MTP_COLs::w0_s)],
                                      Qt::CaseInsensitive)) {
                    mtp.w0_s = item->toDouble();
                }
                if (!QString::compare(headers[_ic], MTP_ColNames[static_cast<int>(MTP_COLs::z0)],
                                      Qt::CaseInsensitive)) {
                    mtp.z0 = item->toDouble();
                }
                if (!QString::compare(headers[_ic], MTP_ColNames[static_cast<int>(MTP_COLs::z3)],
                                      Qt::CaseInsensitive)) {
                    mtp.z3 = item->toDouble();
                }
                if (!QString::compare(headers[_ic], MTP_ColNames[static_cast<int>(MTP_COLs::sigma)],
                                      Qt::CaseInsensitive)) {
                    mtp.sigma = item->toDouble();
                }
                if (!QString::compare(headers[_ic], MTP_ColNames[static_cast<int>(MTP_COLs::delta)],
                                      Qt::CaseInsensitive)) {
                    mtp.delta = item->toDouble();
                }
                if (!QString::compare(headers[_ic], MTP_ColNames[static_cast<int>(MTP_COLs::ispta_s)],
                                      Qt::CaseInsensitive)) {
                    mtp.ispta_s = item->toDouble();
                }
                if (!QString::compare(headers[_ic], MTP_ColNames[static_cast<int>(MTP_COLs::ipa3_mi)],
                                      Qt::CaseInsensitive)) {
                    mtp.ipa3_mi = item->toDouble();
                }
                if (!QString::compare(headers[_ic], MTP_ColNames[static_cast<int>(MTP_COLs::ispta3_s)],
                                      Qt::CaseInsensitive)) {
                    mtp.ispta3_s = item->toDouble();
                }
                if (!QString::compare(headers[_ic], MTP_ColNames[static_cast<int>(MTP_COLs::FL_Azim)],
                                      Qt::CaseInsensitive)) {
                    mtp.FL_Azim = item->toDouble();
                }
                if (!QString::compare(headers[_ic], MTP_ColNames[static_cast<int>(MTP_COLs::FL_Elev)],
                                      Qt::CaseInsensitive)) {
                    mtp.FL_Elev = item->toDouble();
                }
                if (!QString::compare(headers[_ic], MTP_ColNames[static_cast<int>(MTP_COLs::AaptAzim)],
                                      Qt::CaseInsensitive)) {
                    mtp.AaptAzim = item->toDouble();
                }
                if (!QString::compare(headers[_ic], MTP_ColNames[static_cast<int>(MTP_COLs::deq4MI)],
                                      Qt::CaseInsensitive)) {
                    mtp.deq4MI = item->toDouble();
                }
                if (!QString::compare(headers[_ic], MTP_ColNames[static_cast<int>(MTP_COLs::deq4TIB)],
                                      Qt::CaseInsensitive)) {
                    mtp.deq4TIB = item->toDouble();
                }
                if (!QString::compare(headers[_ic], MTP_ColNames[static_cast<int>(MTP_COLs::zB3)],
                                      Qt::CaseInsensitive)) {
                    mtp.zB3 = item->toDouble();
                }
                if (!QString::compare(headers[_ic], MTP_ColNames[static_cast<int>(MTP_COLs::w01_mW)],
                                      Qt::CaseInsensitive)) {
                    mtp.w01_mW = item->toDouble();
                }
                if (!QString::compare(headers[_ic], MTP_ColNames[static_cast<int>(MTP_COLs::zBP_cm)],
                                      Qt::CaseInsensitive)) {
                    mtp.zBP_cm = item->toDouble();
                }
                if (!QString::compare(headers[_ic], MTP_ColNames[static_cast<int>(MTP_COLs::z1_cm)],
                                      Qt::CaseInsensitive)) {
                    mtp.z1_cm = item->toDouble();
                }
                if (!QString::compare(headers[_ic], MTP_ColNames[static_cast<int>(MTP_COLs::minW3ITA3_mW)],
                                      Qt::CaseInsensitive)) {
                    mtp.minW3ITA3_mW = item->toDouble();
                }
                if (!QString::compare(headers[_ic], MTP_ColNames[static_cast<int>(MTP_COLs::ticas_s)],
                                      Qt::CaseInsensitive)) {
                    mtp.ticas_s = item->toDouble();
                }
                if (!QString::compare(headers[_ic], MTP_ColNames[static_cast<int>(MTP_COLs::tisas_s)],
                                      Qt::CaseInsensitive)) {
                    mtp.tisas_s = item->toDouble();
                }
            }
            ret_ok = addRow((void *)&mtp, table_type);
            if (!ret_ok && err_count < 10) {
                qDebug() << _ri << ":]mtp not insert successful:" << _row;
                err_count++;
                ret_ok = true;
            }
            break;
        }
        default:
            qDebug() << "no table match...";
            break;
        }
        if (!ret_ok) {
            return ret_ok;
        }
    }
    return ret_ok;
}

bool APNI_DB_Adapter::addModes(const vector<QStringList> &_rows) { return addRows(_rows, TABLE_MODE); }

bool APNI_DB_Adapter::lookupUtps(int utp_id) {
    bool ret_ok = false;
    odb::core::transaction t(this->begin());
    try {
        odb::result<UTPs> utp_rets(query<UTPs>(odb::query<UTPs>::id_utp >= utp_id));
        int n = utp_rets.size();
        if (n > 0) {
            for (odb::result<UTPs>::iterator i(utp_rets.begin()); i != utp_rets.end(); i++) {
                qDebug() << "look up UTPs:" << i->id_utp;
            }
        }
    } catch (odb::exception &e) {
        qDebug() << "look up UTPs failed." << e.what();
    }

    t.commit();

    return ret_ok;
}
