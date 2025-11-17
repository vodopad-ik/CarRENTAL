#pragma once

#include <QSqlDatabase>
#include <QString>

class CarSeeder {
public:
  void seedIfEmpty(QSqlDatabase &db);
  void populateSpecs(QSqlDatabase &db);

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
  bool insertCar(QSqlDatabase &db, const CarData &data);
  struct CarSpecs {
    QString type;
    double capacity;
    int power;
    int seats;
  };
  CarSpecs determineSpecs(const QString &brand, const QString &model,
                          const QStringList &suvKeys) const;
};









