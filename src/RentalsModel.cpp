#include "RentalsModel.h"
#include "db/Database.h"
#include "utils/CurrencyConverter.h"
#include <QDate>
#include <QSqlQuery>
#include <QVariant>

RentalsModel::RentalsModel(QObject *parent)
    : QSqlQueryModel(parent), currentCurrency_("USD"),
      converter_(&CurrencyConverter::instance()) {
  setCurrency("USD");
}

void RentalsModel::setCurrency(const QString &currency) {
  currentCurrency_ = currency;

  if (rowCount() > 0) {
    emit headerDataChanged(Qt::Horizontal, 4, 4);
    emit dataChanged(index(0, 4), index(rowCount() - 1, 4));
  }
}

void RentalsModel::refresh(int customerId) {
  QString query = QString("SELECT r.id, c.brand || ' ' || c.model as car_name, "
                          "r.start_date, r.end_date, r.total_price, r.status "
                          "FROM rentals r JOIN cars c ON r.car_id = c.id "
                          "WHERE r.customer_id = %1 ORDER BY r.start_date DESC")
                      .arg(customerId);
  setQuery(query, Database::instance().database());


  setHeaderData(1, Qt::Horizontal, "Автомобиль");
  setHeaderData(2, Qt::Horizontal, "Дата начала");
  setHeaderData(3, Qt::Horizontal, "Дата окончания");
  setHeaderData(4, Qt::Horizontal, "Сумма");
  setHeaderData(5, Qt::Horizontal, "Статус");
}

QVariant RentalsModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid())
    return QVariant();


  if (index.column() == 4 && role == Qt::DisplayRole) {
    QVariant baseValue = QSqlQueryModel::data(index, role);
    if (baseValue.isValid()) {
      double priceInUsd = baseValue.toDouble();
      auto currency = CurrencyConverter::fromString(currentCurrency_);
      double convertedPrice = converter_->fromBase(priceInUsd, currency);
      QString symbol = converter_->symbol(currency);

      return QString::number(convertedPrice, 'f', 2) + " " + symbol;
    }
  }


  if (index.column() == 5 && role == Qt::DisplayRole) {
    QVariant baseValue = QSqlQueryModel::data(index, Qt::EditRole);
    QString status = baseValue.toString();


    if (status == "active")
      return "Активна";
    if (status == "expired")
      return "Истекла";
    return status;
  }

  return QSqlQueryModel::data(index, role);
}

QVariant RentalsModel::headerData(int section, Qt::Orientation orientation,
                                  int role) const {
  if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
    if (section == 4) {

      auto currency = CurrencyConverter::fromString(currentCurrency_);
      QString symbol = converter_->symbol(currency);
      QString code = converter_->code(currency);
      return QString("Сумма (%1)").arg(code);
    }
  }
  return QSqlQueryModel::headerData(section, orientation, role);
}
