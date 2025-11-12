#pragma once

#include "CarInfo.h"

#include <QDate>
#include <QSqlDatabase>
#include <QSqlQueryModel>
#include <QString>

#include <memory>

class SchemaManager;
class CarSeeder;
class CarsRepository;
class CustomersRepository;
class RentalsRepository;

class Database {
public:
  static Database &instance();

  bool initialize();


  QSqlQueryModel *getAvailableCars(int customerId);
  QSqlQueryModel *getBookmarked(int customerId);
  CarInfo getCar(int id, int customerId);
  int getAvailableQuantity(int carId, const QDate &startDate,
                           const QDate &endDate);
  bool setBookmarked(int carId, int customerId, bool bookmarked);


  int addCustomer(const QString &name, const QString &phone,
                  const QString &passwordHash);
  int findCustomerByPhoneAndPassword(const QString &phone,
                                     const QString &passwordHash);


  int createRental(int carId, int customerId, const QDate &startDate,
                   const QDate &endDate, double totalPrice);

  void updateExpiredRentals();

  QSqlDatabase &database() { return db_; }

private:
  Database();

  bool createSchema();
  void seedCars();
  void populateCarSpecs();
  void ensureRepositories();

  QSqlDatabase db_;
  std::unique_ptr<SchemaManager> schemaManager_;
  std::unique_ptr<CarSeeder> carSeeder_;
  std::unique_ptr<CarsRepository> carsRepository_;
  std::unique_ptr<CustomersRepository> customersRepository_;
  std::unique_ptr<RentalsRepository> rentalsRepository_;
};
