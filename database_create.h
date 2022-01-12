// file      : hello/database.hxx
// copyright : not copyrighted - public domain

//
// Create concrete database instance based on the DATABASE_* macros.
//

#ifndef DATABASE_HXX
#define DATABASE_HXX

#include <cstdlib> // std::exit
#include <iostream>
#include <memory> // std::auto_ptr
#include <string>

#include <odb/database.hxx>

#if defined(DATABASE_SQLITE)
#include <odb/connection.hxx>
#include <odb/schema-catalog.hxx>
#include <odb/sqlite/database.hxx>
#include <odb/transaction.hxx>

#else
#error unknown database; did you forget to define the DATABASE_* macros?
#endif

inline std::auto_ptr<odb::database> create_database(int &argc, char *argv[]) {
    using namespace std;
    using namespace odb::core;

    if (argc > 1 && argv[1] == string("--help")) {
        cout << "Usage: " << argv[0] << " [options]" << endl << "Options:" << endl;

        odb::sqlite::database::print_usage(cout);

        exit(0);
    }

#if defined(DATABASE_SQLITE)
    auto_ptr<database> db(new odb::sqlite::database(argc, argv, false, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE));

    // Create the database schema. Due to bugs in SQLite foreign key
    // support for DDL statements, we need to temporarily disable
    // foreign keys.
    //
    {
        connection_ptr c(db->connection());

        c->execute("PRAGMA foreign_keys=OFF");

        transaction t(c->begin());
        schema_catalog::create_schema(*db, "etons");

        t.commit();

        c->execute("PRAGMA foreign_keys=ON");
    }
#endif

    return db;
}

#endif // DATABASE_HXX
