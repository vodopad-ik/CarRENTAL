#pragma once

#include <QModelIndex>
#include <QSqlQueryModel>
#include <QString>
#include <QVariant>

class CurrencyConverter;

class RentalsModel : public QSqlQueryModel {
  Q_OBJECT

public:
  explicit RentalsModel(QObject *parent = nullptr);
  void setCurrency(const QString &currency);
  void refresh(int customerId);

  QVariant data(const QModelIndex &index,
                int role = Qt::DisplayRole) const override;
  QVariant headerData(int section, Qt::Orientation orientation,
                      int role = Qt::DisplayRole) const override;

private:
  QString currentCurrency_;
  CurrencyConverter *converter_;
};
