#ifndef APAI_DB_ADAPTER_H
#define APAI_DB_ADAPTER_H
#include "esidatabase.h"
#include <QDebug>
#include <typeinfo>
#include <vector>

#define DB_SCHEMA_NAME "APaI"
#define APAI_DB_VERSION 1

// 0.cid [0,1...]; 1.name [str]; 2.type [str]; 3.notnull[0/1]; 4. dflt_value; 5.pk[]0/1;
enum class SQLITE_TABLE_INFO_IDX { CID = 0, NAME, TYPE, NOTNULL, DFLT_VALUE, PK };

enum class UTP_COLs {
    id_utp = 0,
    id_pulse_type,
    id_probe,
    id_mode,
    id_apodization,
    id_mtp,
    location,
    nb_element,
    freq,
    nb_half_cycle,
    polarity,
    transmit_lines,
    composed_mode,
    duty_cycle,
    nb_element_max,
    voltage,
    delta,
    deltaT,
    manipulated
};

struct TableInfo {
    int cid;
    QString name;
    QString type;
    bool notnull;
    QString dflt_value;
    bool pk;
};

class UTPs;
class MTPs;
class UTPInfos;

class APAI_DB_Adapter : public ESIDatabase {
  public:
    APAI_DB_Adapter(const QString &file_name);

    bool queryTableInfo(const QString &sql, QList<TableInfo> &);
    bool isHeaderMatch(const QStringList &csvHeaders, int);
    void addVersion(const QString &msg);
    bool addProbe(const int &_id, const QString &_name);
    bool addProbes(const vector<int> &_id, const vector<QString> &_name);
    bool addMode(const int &_id, const QString &_name);
    bool addRow(const int &_id, const QString &_name, int);
    bool addRow(const UTPs &, int);
    bool addRow(const MTPs &, int);
    bool addRow(const UTPInfos &, int);
    bool addModes(const vector<int> &_id, const vector<QString> &_name);
    bool addModes(const vector<QStringList> &_rows);
    bool addRows(const vector<QStringList> &_rows, int table_type);

  public:
    enum TABLE_TYPE {
        TABLE_PROBE = 0,
        TABLE_MODE,
        TABLE_APOD,
        TABLE_PULSE,
        TABLE_NULL,
        TABLE_UTP,
        TABLE_MTP,
        TABLE_UTPI,
        TABLE_END
    };

    static const QString TableNames[];
    static const QString UTP_ColNames[];
};

#endif // APAI_DB_ADAPTER_H
