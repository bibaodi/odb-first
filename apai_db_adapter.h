#ifndef APAI_DB_ADAPTER_H
#define APAI_DB_ADAPTER_H
#include "esidatabase.h"
#include <QDebug>
#include <vector>

#define DB_SCHEMA_NAME "APaI"
#define APAI_DB_VERSION 1

class APAI_DB_Adapter : public ESIDatabase {
  public:
    APAI_DB_Adapter(const QString &file_name);
    void addVersion(const QString &msg);
    bool addProbe(const int &_id, const QString &_name);
    bool addProbes(const vector<int> &_id, const vector<QString> &_name);
    bool addMode(const int &_id, const QString &_name);
    bool addRow(const int &_id, const QString &_name, int);
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
};

#endif // APAI_DB_ADAPTER_H
