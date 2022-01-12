#ifndef APAI_DB_ADAPTER_H
#define APAI_DB_ADAPTER_H
#include "esidatabase.h"

#define DB_SCHEMA_NAME "APaI"
#define APAI_DB_VERSION 1

class APAI_DB_Adapter : public ESIDatabase {
  public:
    APAI_DB_Adapter(const QString &file_name);
    void addVersion(const QString &msg);
};

#endif // APAI_DB_ADAPTER_H
