// file      : hello/driver.cxx
// copyright : not copyrighted - public domain

#include <iostream>
#include <memory> // std::auto_ptr

#include <odb/database.hxx>
#include <odb/transaction.hxx>

#include "database_create.h" // create_database

#include "person-odb.hxx"
#include "person.h"
//#include "person_odb.h"

using namespace std;
using namespace odb::core;

int main_db(int argc, char *argv[]) {
    try {
        auto_ptr<database> db(create_database(argc, argv));

        unsigned long john_id, joe_id;

        // Create a few persistent Person objects.
        //
        odb::schema_version ver = db->schema_version("etons");
        if (!ver) {
            Person john("John", "Doe", 33);
            Person jane("Jane", "Doe", 32);
            Person joe("Joe", "Dirt", 30);

            transaction t(db->begin());

            // Make objects persistent and save their ids for later use.
            //
            john_id = db->persist(john);
            db->persist(jane);
            joe_id = db->persist(joe);

            t.commit();
        }

        //        typedef odb::query<Person> query;
        //        typedef odb::result<Person> result;

        // Say hello to those over 30.
        //
        {
            transaction t(db->begin());

            odb::result<Person> r(db->query<Person>(odb::query<Person>::age > 30));

            for (odb::result<Person>::iterator i(r.begin()); i != r.end(); ++i) {
                cout << "Hello, " << i->first() << " " << i->last() << "!" << endl;
            }

            t.commit();
        }

        // Joe Dirt just had a birthday, so update his age.
        //
        {
            transaction t(db->begin());

            auto_ptr<Person> joe(db->load<Person>(joe_id));
            joe->age(joe->age() + 1);
            db->update(*joe);

            t.commit();
        }

        // Alternative implementation without using the id.
        //
        /*
        {
          transaction t (db->begin ());

          // Here we know that there can be only one Joe Dirt in our
          // database so we use the query_one() shortcut instead of
          // manually iterating over the result returned by query().
          //
          auto_ptr<Person> joe (
            db->query_one<Person> (query::first == "Joe" &&
                                   query::last == "Dirt"));

          if (joe.get () != 0)
          {
            joe->age (joe->age () + 1);
            db->update (*joe);
          }

          t.commit ();
        }
        */

        // Print some statistics about all the people in our database.
        //
        {
            transaction t(db->begin());

            // The result of this (aggregate) query always has exactly one element
            // so use the query_value() shortcut.
            //
            person_stat ps(db->query_value<person_stat>());

            cout << endl
                 << "count  : " << ps.count << endl
                 << "min age: " << ps.min_age << endl
                 << "max age: " << ps.max_age << endl;

            t.commit();
        }

        // John Doe is no longer in our database.
        //
        {
            transaction t(db->begin());
            db->erase<Person>(john_id);
            t.commit();
        }
    } catch (const odb::exception &e) {
        cerr << e.what() << endl;
        return 1;
    }
}

#include <QGuiApplication>
#include <QQmlApplicationEngine>

int main(int argc, char *argv[]) {
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(
        &engine, &QQmlApplicationEngine::objectCreated, &app,
        [url](QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        },
        Qt::QueuedConnection);
    engine.load(url);
    main_db(argc, argv);
    return app.exec();
}
