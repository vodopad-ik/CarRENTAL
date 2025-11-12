#pragma once

#include <QDate>
#include <QSqlDatabase>

class RentalsRepository {
public:
  explicit RentalsRepository(QSqlDatabase &db);

  int createRental(int carId, int customerId, const QDate &startDate,
                   const QDate &endDate, double totalPrice) const;
  void updateExpiredRentals() const;

private:
  QSqlDatabase &db_;
};

