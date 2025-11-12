#pragma once

#include <QSqlDatabase>

class CarSeeder {
public:
  void seedIfEmpty(QSqlDatabase &db);
  void populateSpecs(QSqlDatabase &db);

private:
  bool hasCars(QSqlDatabase &db) const;
  bool insertCar(QSqlDatabase &db, const QString &brand, const QString &model,
                 int year, double pricePerDay, int quantity,
                 const QString &description, const QString &imagePath);
};

