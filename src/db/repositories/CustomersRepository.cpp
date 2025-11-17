#include "CustomersRepository.h"

#include "exceptions/DatabaseException.h"

#include <QSqlError>
#include <QSqlQuery>

CustomersRepository::CustomersRepository(QSqlDatabase &db) : db_(db) {}

int CustomersRepository::addCustomer(const QString &name, const QString &phone,
                                     const QString &passwordHash) const {
  QSqlQuery q(db_);
  q.prepare("INSERT INTO customers (name, phone, password_hash) VALUES (?, ?, ?)");
  q.addBindValue(name);
  q.addBindValue(phone);
  q.addBindValue(passwordHash);
  if (!q.exec()) {
    throw DatabaseException("add_customer", q.lastError().text());
  }
  return q.lastInsertId().toInt();
}

int CustomersRepository::findCustomerByPhoneAndPassword(
    const QString &phone, const QString &passwordHash) const {
  QSqlQuery q(db_);
  q.prepare("SELECT id FROM customers WHERE phone = ? AND password_hash = ?");
  q.addBindValue(phone);
  q.addBindValue(passwordHash);
  if (q.exec() && q.next())
    return q.value(0).toInt();
  return -1;
}





