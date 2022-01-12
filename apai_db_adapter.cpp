#include "apai_db_adapter.h"
#include "apai_db-odb.hxx"
#include "apai_db.h"

APAI_DB_Adapter::APAI_DB_Adapter(const QString &file_name) : ESIDatabase(file_name, DB_SCHEMA_NAME, APAI_DB_VERSION) {}

void APAI_DB_Adapter::addVersion(const QString &msg) {

    // transaction tr(beginTrans());
    beginTrans();
    Versions _version(msg);
    persist<Versions>(_version);

    commitTrans();
}
