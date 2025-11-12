#include "Database.h"

#include "CarSeeder.h"
#include "SchemaManager.h"
#include "repositories/CarsRepository.h"
#include "repositories/CustomersRepository.h"
#include "repositories/RentalsRepository.h"

#include "utils/PathsConfig.h"

#include <QDir>

Database &Database::instance() {
  static Database inst;
  return inst;
}

Database::Database() = default;

bool Database::initialize() {
  if (!QSqlDatabase::contains("carrental")) {
    db_ = QSqlDatabase::addDatabase("QSQLITE", "carrental");
  } else {
    db_ = QSqlDatabase::database("carrental");
  }

  const QString dataDir = PathsConfig::dataDir();
  const QString dbPath = PathsConfig::databaseFile();
  db_.setDatabaseName(dbPath);

  if (!db_.open()) {
    return false;
  }

  if (!createSchema())
    return false;

  seedCars();
  populateCarSpecs();

  ensureRepositories();
  return true;
}

bool Database::createSchema() {
  if (!schemaManager_)
    schemaManager_ = std::make_unique<SchemaManager>();
  return schemaManager_->ensureSchema(db_);
}

void Database::seedCars() {
  if (!carSeeder_)
    carSeeder_ = std::make_unique<CarSeeder>();
  carSeeder_->seedIfEmpty(db_);
  }

void Database::populateCarSpecs() {
  if (!carSeeder_)
    carSeeder_ = std::make_unique<CarSeeder>();
  carSeeder_->populateSpecs(db_);
}

void Database::ensureRepositories() {
  if (!carsRepository_)
    carsRepository_ = std::make_unique<CarsRepository>(db_);
  if (!customersRepository_)
    customersRepository_ = std::make_unique<CustomersRepository>(db_);
  if (!rentalsRepository_)
    rentalsRepository_ = std::make_unique<RentalsRepository>(db_);
}

QSqlQueryModel *Database::getAvailableCars(int customerId) {
  ensureRepositories();
  return carsRepository_->fetchAvailableCars(customerId);
}

QSqlQueryModel *Database::getBookmarked(int customerId) {
  ensureRepositories();
  return carsRepository_->fetchBookmarkedCars(customerId);
}

CarInfo Database::getCar(int id, int customerId) {
  ensureRepositories();
  return carsRepository_->findCar(id, customerId);
}

int Database::getAvailableQuantity(int carId, const QDate &startDate,
                                   const QDate &endDate) {
  ensureRepositories();
  return carsRepository_->availableQuantity(carId, startDate, endDate);
}

bool Database::setBookmarked(int carId, int customerId, bool bookmarked) {
  ensureRepositories();
  return carsRepository_->setBookmarked(carId, customerId, bookmarked);
}

int Database::addCustomer(const QString &name, const QString &phone,
                          const QString &passwordHash) {
  ensureRepositories();
  return customersRepository_->addCustomer(name, phone, passwordHash);
}

int Database::findCustomerByPhoneAndPassword(const QString &phone,
                                             const QString &passwordHash) {
  ensureRepositories();
  return customersRepository_->findCustomerByPhoneAndPassword(phone,
                                                              passwordHash);
}

int Database::createRental(int carId, int customerId, const QDate &startDate,
                           const QDate &endDate, double totalPrice) {
  ensureRepositories();
  return rentalsRepository_->createRental(carId, customerId, startDate, endDate,
                                          totalPrice);
}

void Database::updateExpiredRentals() {
  ensureRepositories();
  rentalsRepository_->updateExpiredRentals();
}
