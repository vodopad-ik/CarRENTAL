#include "SchemaManager.h"

#include "exceptions/DatabaseException.h"

#include <QSqlError>
#include <QSqlQuery>

bool SchemaManager::ensureSchema(const QSqlDatabase &db) {
  if (!createTables(db))
    return false;
  createIndexes(db);
  return true;
}

bool SchemaManager::createTables(const QSqlDatabase &db) {
  QSqlQuery q(db);

  if (!q.exec("CREATE TABLE IF NOT EXISTS cars ("
              "id INTEGER PRIMARY KEY AUTOINCREMENT,"
              "brand TEXT NOT NULL,"
              "model TEXT NOT NULL,"
              "year INTEGER NOT NULL,"
              "price_per_day REAL NOT NULL,"
              "quantity INTEGER DEFAULT 1,"
              "description TEXT,"
              "available INTEGER DEFAULT 1,"
              "image_path TEXT,"
              "bookmarked INTEGER DEFAULT 0,"
              "engine_type TEXT DEFAULT '',"
              "engine_capacity REAL DEFAULT 0,"
              "power_hp INTEGER DEFAULT 0,"
              "seats INTEGER DEFAULT 0"
              ")")) {
    throw DatabaseException("create_table_cars", q.lastError().text());
  }

  if (!q.exec("CREATE TABLE IF NOT EXISTS customers ("
              "id INTEGER PRIMARY KEY AUTOINCREMENT,"
              "name TEXT NOT NULL,"
              "phone TEXT NOT NULL,"
              "password_hash TEXT"
              ")")) {
    throw DatabaseException("create_table_customers", q.lastError().text());
  }

  if (!q.exec("CREATE TABLE IF NOT EXISTS bookmarks ("
              "id INTEGER PRIMARY KEY AUTOINCREMENT,"
              "customer_id INTEGER NOT NULL,"
              "car_id INTEGER NOT NULL,"
              "UNIQUE(customer_id, car_id),"
              "FOREIGN KEY(customer_id) REFERENCES customers(id),"
              "FOREIGN KEY(car_id) REFERENCES cars(id)"
              ")")) {
    throw DatabaseException("create_table_bookmarks", q.lastError().text());
  }

  if (!q.exec("CREATE TABLE IF NOT EXISTS rentals ("
              "id INTEGER PRIMARY KEY AUTOINCREMENT,"
              "car_id INTEGER NOT NULL,"
              "customer_id INTEGER NOT NULL,"
              "start_date TEXT NOT NULL,"
              "end_date TEXT NOT NULL,"
              "total_price REAL NOT NULL,"
              "status TEXT DEFAULT 'active',"
              "FOREIGN KEY(car_id) REFERENCES cars(id),"
              "FOREIGN KEY(customer_id) REFERENCES customers(id)"
              ")")) {
    throw DatabaseException("create_table_rentals", q.lastError().text());
  }

  return true;
}

void SchemaManager::createIndexes(const QSqlDatabase &db) {
  QSqlQuery uq(db);
  uq.exec("CREATE UNIQUE INDEX IF NOT EXISTS idx_customers_phone_unique ON "
          "customers(phone)");
}





