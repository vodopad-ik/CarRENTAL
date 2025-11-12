#pragma once

#include <QSqlDatabase>

class CustomersRepository {
public:
  explicit CustomersRepository(QSqlDatabase &db);

  int addCustomer(const QString &name, const QString &phone,
                  const QString &passwordHash) const;
  int findCustomerByPhoneAndPassword(const QString &phone,
                                     const QString &passwordHash) const;

private:
  QSqlDatabase &db_;
};

