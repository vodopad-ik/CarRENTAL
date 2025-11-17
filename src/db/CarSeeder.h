#pragma once

#include <QSqlDatabase>

class CarSeeder {
public:
  void seedIfEmpty(const QSqlDatabase &db);
  void populateSpecs(const QSqlDatabase &db);

private:
  bool hasCars(const QSqlDatabase &db) const;
  bool insertCar(const QSqlDatabase &db, const QString &brand, const QString &model,
                 int year, double pricePerDay, int quantity,
                 const QString &description, const QString &imagePath);
  struct CarSpecs {
    QString type;
    double capacity;
    int power;
    int seats;
  };
  CarSpecs determineSpecs(const QString &brand, const QString &model,
                          const QStringList &suvKeys) const;
};









