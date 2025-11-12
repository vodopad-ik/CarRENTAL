#include "CarsRepository.h"

#include <QSqlQuery>

CarsRepository::CarsRepository(QSqlDatabase &db) : db_(db) {}

QSqlQueryModel *CarsRepository::fetchAvailableCars(int customerId) const {
  auto *model = new QSqlQueryModel();
  QSqlQuery q(db_);
  q.prepare("SELECT c.id, c.brand, c.model, c.year, c.price_per_day, "
            "c.description, c.available, c.image_path, "
            "CASE WHEN b.id IS NOT NULL THEN 1 ELSE 0 END AS bookmarked, "
            "c.engine_type, c.engine_capacity, c.power_hp, c.seats, c.quantity "
            "FROM cars c "
            "LEFT JOIN bookmarks b ON c.id = b.car_id AND b.customer_id = ? "
            "WHERE c.available = 1 ORDER BY c.brand, c.model");
  q.addBindValue(customerId);
  q.exec();
  model->setQuery(std::move(q));
  return model;
}

QSqlQueryModel *CarsRepository::fetchBookmarkedCars(int customerId) const {
  auto *model = new QSqlQueryModel();
  QSqlQuery q(db_);
  q.prepare("SELECT c.id, c.brand, c.model, c.year, c.price_per_day, "
            "c.description, c.available, c.image_path, 1 AS bookmarked, "
            "c.engine_type, c.engine_capacity, c.power_hp, c.seats, c.quantity "
            "FROM cars c JOIN bookmarks b ON c.id = b.car_id "
            "WHERE b.customer_id = ? ORDER BY c.brand, c.model");
  q.addBindValue(customerId);
  q.exec();
  model->setQuery(std::move(q));
  return model;
}

CarInfo CarsRepository::findCar(int id, int customerId) const {
  CarInfo info{};
  QSqlQuery q(db_);
  q.prepare("SELECT c.id, c.brand, c.model, c.year, c.price_per_day, c.quantity, "
            "c.description, c.available, c.image_path, "
            "CASE WHEN b.id IS NOT NULL THEN 1 ELSE 0 END AS bookmarked, "
            "c.engine_type, c.engine_capacity, c.power_hp, c.seats "
            "FROM cars c LEFT JOIN bookmarks b ON c.id = b.car_id AND "
            "b.customer_id = ? WHERE c.id = ?");
  q.addBindValue(customerId);
  q.addBindValue(id);
  if (q.exec() && q.next()) {
    info.id = q.value(0).toInt();
    info.brand = q.value(1).toString();
    info.model = q.value(2).toString();
    info.year = q.value(3).toInt();
    info.pricePerDay = q.value(4).toDouble();
    info.quantity = q.value(5).toInt();
    info.description = q.value(6).toString();
    info.available = q.value(7).toInt() == 1;
    info.imagePath = q.value(8).toString();
    info.bookmarked = q.value(9).toInt() == 1;
    info.engineType = q.value(10).toString();
    info.engineCapacityL = q.value(11).toDouble();
    info.powerHp = q.value(12).toInt();
    info.seats = q.value(13).toInt();
  }
  return info;
}

int CarsRepository::availableQuantity(int carId, const QDate &startDate,
                                      const QDate &endDate) const {
  QSqlQuery q(db_);
  q.prepare("SELECT quantity - (SELECT COUNT(*) FROM rentals WHERE car_id = ? "
            "AND NOT(date(end_date) <= date(?) OR date(start_date) >= date(?)))"
            " FROM cars WHERE id=?");
  q.addBindValue(carId);
  q.addBindValue(startDate.toString("yyyy-MM-dd"));
  q.addBindValue(endDate.toString("yyyy-MM-dd"));
  q.addBindValue(carId);
  if (q.exec() && q.next())
    return q.value(0).toInt();
  return 0;
}

bool CarsRepository::setBookmarked(int carId, int customerId,
                                   bool bookmarked) const {
  if (bookmarked) {
    QSqlQuery q(db_);
    q.prepare("INSERT OR IGNORE INTO bookmarks (customer_id, car_id) VALUES (?, ?)" );
    q.addBindValue(customerId);
    q.addBindValue(carId);
    return q.exec();
  }

  QSqlQuery q(db_);
  q.prepare("DELETE FROM bookmarks WHERE customer_id=? AND car_id=?");
  q.addBindValue(customerId);
  q.addBindValue(carId);
  return q.exec();
}

