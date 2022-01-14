#include "apai_db_adapter.h"
#include "apai_db-odb.hxx"
#include "apai_db.h"
#include <odb/sqlite/exceptions.hxx>

APAI_DB_Adapter::APAI_DB_Adapter(const QString &file_name) : ESIDatabase(file_name, DB_SCHEMA_NAME, APAI_DB_VERSION) {}

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
    for (int i = 0; i < _rows.size(); i++) {
        qDebug() << "row=" << _rows[i];
        _row = _rows[i];
        if (_row.length() < 2) {
            return false;
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
