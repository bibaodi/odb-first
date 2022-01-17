#include "apai_db_adapter.h"
#include "apai_db-odb.hxx"
#include "apai_db.h"
#include <QTextCodec>
#include <iostream>
#include <odb/sqlite/exceptions.hxx>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>

#define matchHeader(x) QString member_name(#x)

const QString APAI_DB_Adapter::TableNames[] = {
    QStringLiteral("Probes"),     QStringLiteral("Modes"),    QStringLiteral("Apodizations"),
    QStringLiteral("PulseTypes"), QStringLiteral("null"),     QStringLiteral("UTPs"),
    QStringLiteral("MTPs"),       QStringLiteral("UTPInfos"), QLatin1Literal()};

const QString APAI_DB_Adapter::UTP_ColNames[] = {
    "id_utp",         " id_pulse_type", "id_probe",      "id_mode",  "id_apodization", "id_mtp",        "location",
    "nb_element",     "freq",           "nb_half_cycle", "polarity", "transmit_lines", "composed_mode", "duty_cycle",
    "nb_element_max", "voltage",        "delta",         "deltaT",   "manipulated",
};

APAI_DB_Adapter::APAI_DB_Adapter(const QString &file_name) : ESIDatabase(file_name, DB_SCHEMA_NAME, APAI_DB_VERSION) {}

bool APAI_DB_Adapter::isHeaderMatch(const QStringList &csvHeaders, int table_type) {
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

bool APAI_DB_Adapter::queryTableInfo(const QString &tb_name, QList<TableInfo> &tbinfos) {
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

void APAI_DB_Adapter::addVersion(const QString &msg) {
    // transaction tr(beginTrans());
    beginTrans();
    Versions _version(msg);
    persist<Versions>(_version);
    commitTrans();
}

bool APAI_DB_Adapter::addProbe(const int &_id, const QString &_name) {
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

bool APAI_DB_Adapter::addProbes(const vector<int> &_ids, const vector<QString> &_names) {
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

bool APAI_DB_Adapter::addMode(const int &_id, const QString &_name) {
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

bool APAI_DB_Adapter::addModes(const vector<int> &_ids, const vector<QString> &_names) {
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

bool APAI_DB_Adapter::addRow(const UTPs &utp, int table_type) {
    bool ret_ok = false;
    ret_ok = beginTrans();
    if (!ret_ok) {
        return ret_ok;
    }

    try {
        persist<UTPs>(utp);
    } catch (const odb::exception &e) {
        qDebug() << e.what();
        return false;
    }

    ret_ok = commitTrans();

    return ret_ok;
}
bool APAI_DB_Adapter::addRow(const MTPs &, int) { return true; }
bool APAI_DB_Adapter::addRow(const UTPInfos &, int) { return true; }

bool APAI_DB_Adapter::addRow(const int &_id, const QString &_name, int table_type) {
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

bool APAI_DB_Adapter::addRows(const vector<QStringList> &_rows, int table_type) {
    QStringList _row = _rows[0];
    bool ret_ok = false;
    int err_count = 0;

    ret_ok = isHeaderMatch(_row, table_type);
    if (!ret_ok) {
        qDebug() << "Header not match~!" << TableNames[table_type];
        return false;
    }
    const QStringList headers = _rows[0];

    for (int i = 1; i < _rows.size(); i++) {
        if (i < 10) {
            qDebug() << "row=" << _rows[i] << "::" << i;
        }
        _row = _rows[i];
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
            }
            ret_ok = addRow(utp, table_type);
            if (!ret_ok && err_count < 10) {
                qDebug() << i << ":]utp not insert successful:" << _row;
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

bool APAI_DB_Adapter::addModes(const vector<QStringList> &_rows) { return addRows(_rows, TABLE_MODE); }
