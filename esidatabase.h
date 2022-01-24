#ifndef ESIDATABASE_H
#define ESIDATABASE_H
#include <QString>
#include <memory>
//#include <odb/database.hxx>
#include <odb/sqlite/database.hxx>
// using namespace std;
// using namespace odb::core;
class QTextCodec;
class ESIDatabase : public odb::sqlite::database {
  public:
    ESIDatabase(const QString &db_name, const QString &schema_name, int cur_version);
    ~ESIDatabase();
    bool beginTrans();
    bool commitTrans();
    const QTextCodec *get_codec(void) { return m_codec; }
    sqlite3 *get_handler(void) { return m_dbHandler; }
    int init_db(int _version);

  private:
    bool isTableExist(const QString &table_name);
    void deleteDbTrans();
    virtual void upgrade(int cur_version, int db_version, const QString &schema_name);
    virtual void degrade(int cur_version, int db_version, const QString &schema_name);

  private:
    sqlite3 *m_dbHandler = nullptr;
    QTextCodec *m_codec = nullptr;
    odb::sqlite::transaction *m_dbTransaction = nullptr;
    QString m_dbFileName;

    int m_preDBVersion;
    bool m_update = false;
    int m_lstCommitResult = SQLITE_OK;
    QString m_schemaName;

  protected:
    bool m_available;
};

#endif // ESIDATABASE_H
