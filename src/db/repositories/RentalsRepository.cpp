#include "RentalsRepository.h"

#include <QSqlQuery>

RentalsRepository::RentalsRepository(QSqlDatabase &db) : db_(db) {}

int RentalsRepository::createRental(int carId, int customerId,
                                    const QDate &startDate,
                                    const QDate &endDate,
                                    double totalPrice) const {
  QSqlQuery q(db_);
  q.prepare("INSERT INTO rentals (car_id, customer_id, start_date, end_date, "
            "total_price) VALUES (?, ?, ?, ?, ?)");
  q.addBindValue(carId);
  q.addBindValue(customerId);
  q.addBindValue(startDate.toString("yyyy-MM-dd"));
  q.addBindValue(endDate.toString("yyyy-MM-dd"));
  q.addBindValue(totalPrice);
  if (!q.exec())
    return -1;
  return q.lastInsertId().toInt();
}

void RentalsRepository::updateExpiredRentals() const {
  QSqlQuery q(db_);
  q.prepare("UPDATE rentals SET status='expired' WHERE status='active' AND "
            "date(end_date) < date('now')");
  q.exec();
}

