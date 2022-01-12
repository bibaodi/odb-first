#include "esidatabase.h"
#include <QTextCodec>
#include <glog/logging.h>
#include <odb/schema-catalog.hxx>
#include <odb/sqlite/database.hxx>
#include <odb/sqlite/exceptions.hxx>
const char *SCHEMA_VERTION_TABLE = "schema_version";
ESIDatabase::ESIDatabase(const QString &db_file, const QString &schema_name, int cur_version)
    : odb::sqlite::database(db_file.toStdString(), SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE), m_dbFileName(db_file) {
    m_dbHandler = this->connection()->handle();
    m_codec = QTextCodec::codecForName("UTF-8");
    //检查数据库文件是否存在
    odb::database::schema_version_type db_version = 0;
    if (isTableExist(SCHEMA_VERTION_TABLE)) {
        db_version = this->schema_version(schema_name.toStdString());
        m_preDBVersion = db_version;
    }
    //数据库文件不存在，根据schema创建
    if (0 == db_version) {
        //对SQLITE支持有个BUG
        this->connection()->execute("PRAGMA foreign_keys=OFF");
        transaction tr(this->begin());
        schema_catalog::create_schema(*this, schema_name.toStdString());
        tr.commit();
        this->connection()->execute("PRAGMA foreign_keys=ON");
    }
    //数据库版本不一致
    else {
        //升级
        if (cur_version > db_version) {
            upgrade(cur_version, db_version, schema_name);
        }
        //降级
        else if (cur_version < db_version) {
            degrade(cur_version, db_version, schema_name);
        }
    }
}

ESIDatabase::~ESIDatabase() {
    deleteDbTrans();
    sqlite3_close(m_dbHandler);
}

void ESIDatabase::upgrade(int cur_version, int db_version, const QString &schema_name) {
    transaction tr(this->connection()->begin());
    schema_catalog::migrate(*this, cur_version, schema_name.toStdString());
    tr.commit();
    m_update = true;
}
//数据库降级，基类中不支持，如果需要可以在子类中重载进行实现
void ESIDatabase::degrade(int cur_version, int db_version, const QString &schema_name) {
    Q_UNUSED(cur_version);
    Q_UNUSED(db_version);
    Q_UNUSED(schema_name);
    Q_ASSERT(false);
}

bool ESIDatabase::isTableExist(const QString &table_name) {
    try {
        char **result = nullptr;
        int cols;
        int rows;
        QString sql_str = "SELECT * FROM sqlite_master WHERE tbl_name = \'" + table_name + "\'";
        QByteArray str = m_codec->fromUnicode(sql_str);
        int rc;
        rc = sqlite3_get_table(m_dbHandler, str.data(), &result, &rows, &cols, nullptr);
        sqlite3_free_table(result);
        return (rows > 0);
    } catch (...) {
        return false;
    }
}

bool ESIDatabase::beginTrans() {
    bool result = true;
    if (nullptr == m_dbTransaction) {
        try {
            m_dbTransaction = new odb::sqlite::transaction(begin_immediate());
        } catch (const odb::sqlite::database_exception &e) {
            m_lstCommitResult = e.error();
            result = false;
            deleteDbTrans();
        }
    }
    return result;
}

bool ESIDatabase::commitTrans() {
    bool b_result = true;
    m_lstCommitResult = SQLITE_OK;
    if (nullptr == m_dbTransaction) {
        return false;
    }
    try {
        m_dbTransaction->commit();
    } catch (const odb::sqlite::database_exception &e) {
        m_lstCommitResult = e.error();
        b_result = false;
    }
    deleteDbTrans();
    return b_result;
}

void ESIDatabase::deleteDbTrans() {
    if (m_dbTransaction) {
        delete m_dbTransaction;
        m_dbTransaction = nullptr;
    }
}
