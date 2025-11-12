#pragma once

#include <QSqlDatabase>

class SchemaManager {
public:
  bool ensureSchema(QSqlDatabase &db);

private:
  bool createTables(QSqlDatabase &db);
  void createIndexes(QSqlDatabase &db);
};

