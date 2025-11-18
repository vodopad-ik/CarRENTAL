#pragma once

#include <QSqlDatabase>
#include <QString>

class CarSeeder {
public:
  void seedIfEmpty(const QSqlDatabase &db) const;
  void populateSpecs(const QSqlDatabase &db) const;

private:
  bool hasCars(const QSqlDatabase &db) const;
  struct CarData {
    QString brand;
    QString model;
    int year;
    double pricePerDay;
    int quantity;
    QString description;
    QString imagePath;
  };
  bool insertCar(const QSqlDatabase &db, const CarData &data) const;
  struct CarSpecs {
    QString type;
    double capacity;
    int power;
    int seats;
  };
  CarSpecs determineSpecs(const QString &brand, const QString &model,
                          const QStringList &suvKeys) const;
};









