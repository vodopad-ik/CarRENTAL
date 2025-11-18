#pragma once

#include <QSqlDatabase>

class SchemaManager {
public:
  bool ensureSchema(const QSqlDatabase &db) const;

private:
  bool createTables(const QSqlDatabase &db) const;
  void createIndexes(const QSqlDatabase &db) const;
};









