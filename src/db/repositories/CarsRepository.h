#pragma once

#include "db/CarInfo.h"

#include <QDate>
#include <QSqlDatabase>
#include <QSqlQueryModel>

class CarsRepository {
public:
  explicit CarsRepository(QSqlDatabase &db);

  QSqlQueryModel *fetchAvailableCars(int customerId) const;
  QSqlQueryModel *fetchBookmarkedCars(int customerId) const;
  CarInfo findCar(int id, int customerId) const;
  int availableQuantity(int carId, const QDate &startDate,
                        const QDate &endDate) const;
  bool setBookmarked(int carId, int customerId, bool bookmarked) const;

private:
  QSqlDatabase &db_;
};

