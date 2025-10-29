#pragma once

#include <QDate>
#include <QSqlDatabase>
#include <QSqlQueryModel>
#include <QSqlTableModel>
#include <QString>
#include <memory>

struct CarInfo {
  int id;
  QString brand;
  QString model;
  int year;
  double pricePerHour;
  QString description;
  bool available;
  QString imagePath;
};

struct CustomerInfo {
  int id;
  QString name;
  QString phone;
  QString email;
};

struct RentalInfo {
  int id;
  int carId;
  QString carName;
  int customerId;
  QString customerName;
  QDate startDate;
  QDate endDate;
  double totalPrice;
  QString status; // "active", "completed", "cancelled"
};

class Database {
public:
  static Database &instance();

  bool initialize();

  // Cars
  QSqlQueryModel *getAvailableCars();
  CarInfo getCar(int id);
  bool isCarAvailable(int carId, const QDate &startDate, const QDate &endDate);

  // Customers
  int addCustomer(const QString &name, const QString &phone,
                  const QString &email);
  CustomerInfo getCustomer(int id);
  QSqlQueryModel *getCustomersModel();

  // Rentals
  int createRental(int carId, int customerId, const QDate &startDate,
                   const QDate &endDate, double totalPrice);
  QSqlQueryModel *getCustomerRentals(int customerId);
  QSqlQueryModel *getAllRentals();
  bool cancelRental(int rentalId);
  bool completeRental(int rentalId);

  // Admin functions for cars
  QSqlTableModel *carsModel();
  bool insertCar(const QString &brand, const QString &model, int year,
                 double pricePerHour, const QString &description = "",
                 const QString &imagePath = "");
  bool updateCar(int id, const QString &brand, const QString &model, int year,
                 double pricePerHour, const QString &description = "",
                 const QString &imagePath = "");
  bool deleteCar(int id);

private:
  Database();
  bool createSchema();
  void setupCarsModel();

  QSqlDatabase db_;
  std::unique_ptr<QSqlTableModel> carsModel_;
};
