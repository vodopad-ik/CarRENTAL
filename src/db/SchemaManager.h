#pragma once

#include <QSqlDatabase>

class SchemaManager {
public:
  bool ensureSchema(const QSqlDatabase &db);

private:
  bool createTables(const QSqlDatabase &db);
  void createIndexes(const QSqlDatabase &db);
};









