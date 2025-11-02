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
  double pricePerDay; // цена за день
  int quantity = 1;   // количество экземпляров (1-10)
  QString description;
  bool available;
  QString imagePath;
  bool bookmarked = false;
  // extra specs
  QString engineType;           // "EV", "Hybrid", "ICE"
  double engineCapacityL = 0.0; // литры
  int powerHp = 0;              // л.с.
  int seats = 0;                // число мест
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
  QSqlQueryModel *getAvailableCars(int customerId = -1);
  QSqlQueryModel *getBookmarkedCars(int customerId);
  CarInfo getCar(int id, int customerId = -1);
  bool isCarAvailable(int carId, const QDate &startDate, const QDate &endDate);
  int getAvailableQuantity(
      int carId, const QDate &startDate,
      const QDate &endDate); // Возвращает доступное количество на период
  bool setBookmarked(int carId, int customerId, bool bookmarked);

  // Customers
  int addCustomer(const QString &name, const QString &phone,
                  const QString &passwordHash);
  CustomerInfo getCustomer(int id);
  QSqlQueryModel *getCustomersModel();
  int findCustomerByPhoneAndPassword(const QString &phone,
                                     const QString &passwordHash);

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
                 double pricePerDay, int quantity = 1,
                 const QString &description = "",
                 const QString &imagePath = "");
  bool updateCar(int id, const QString &brand, const QString &model, int year,
                 double pricePerDay, int quantity = 1,
                 const QString &description = "",
                 const QString &imagePath = "");
  bool deleteCar(int id);

private:
  Database();
  bool createSchema();
  void ensureImagePaths();
  void setupCarsModel();

  QSqlDatabase db_;
  std::unique_ptr<QSqlTableModel> carsModel_;
};
