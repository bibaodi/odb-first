#ifndef APnI_DB_ADAPTER_H
#define APnI_DB_ADAPTER_H
#include "esidatabase.h"
#include <QDebug>
#include <typeinfo>
#include <vector>

#define DB_SCHEMA_NAME "APnI" ///!!!###!!!SCHEMA_NAME APnI should same with CMakefile's .
#define APnI_DB_VERSION 1

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

enum class UTPINFO_COLs {
    id_utp = 0,
    framerate_multiplier,
    framerate_multiplier_m,
    max_voltage,
    voltage,
    mi,
    tib,
    tis,
    ispta,
    valid,
    presets,
    tic,
    settings
};

enum class MTP_COLs {
    id_mtp = 0,
    id_utp,
    T,
    mi,
    mi_inv,
    pii_0_u,
    pii_3_u,
    pii_0_s,
    pii_3_s,
    pii_3_inv_u,
    pii_3_inv_s,
    w0,
    tisas_u,
    tisas_s_factor,
    tisbs_u,
    tibbs_u,
    ticas_u,
    ticas_s_factor,
    td_u,
    td_s,
    ec_hv,
    pd,
    pr,
    pc,
    isppa,
    ispta,
    fc3,
    fc6,
    pr3,
    tisbs_s,
    tibbs_s,
    w0_s,
    z0,
    z3,
    sigma,
    delta,
    ispta_s,
    ipa3_mi,
    ispta3_s,
    FL_Azim,
    FL_Elev,
    AaptAzim,
    deq4MI,
    deq4TIB,
    zB3,
    w01_mW,
    zBP_cm,
    z1_cm,
    minW3ITA3_mW,
    ticas_s,
    tisas_s,
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

class APNI_DB_Adapter : public ESIDatabase {
  private:
    APNI_DB_Adapter(const QString &file_name);
    explicit APNI_DB_Adapter();

  public:
    static APNI_DB_Adapter *get_instance();
    bool queryTableInfo(const QString &sql, QList<TableInfo> &);
    bool isHeaderMatch(const QStringList &csvHeaders, int);
    void addVersion(const QString &msg);
    bool addProbe(const int &_id, const QString &_name);
    bool addProbes(const std::vector<int> &_id, const std::vector<QString> &_name);
    bool addMode(const int &_id, const QString &_name);
    bool addRow(const int &_id, const QString &_name, int);
    bool addUtpRow(const UTPs &);
    bool addMtpRow(const MTPs &);
    bool addUtpiRow(const UTPInfos &);
    bool addRow(void *, int);
    bool addModes(const std::vector<int> &_id, const std::vector<QString> &_name);
    bool addModes(const std::vector<QStringList> &_rows);
    bool addRows(const std::vector<QStringList> &_rows, int table_type);
    bool isAvailable();
    bool lookupUtps(int _id, UTPs &);

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
    static const QString UTPINFO_ColNames[];
    static const QString MTP_ColNames[];
    static const int PROBENAMELEN = 12;
    static const QString ProbesNames[PROBENAMELEN];
};

#endif // APnI_DB_ADAPTER_H
