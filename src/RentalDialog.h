#pragma once

#include <QDateTimeEdit>
#include <QDialog>
#include <QLabel>
#include <QString>

struct CarInfo;

class RentalDialog : public QDialog {
  Q_OBJECT

public:
  explicit RentalDialog(const CarInfo &car, int customerId,
                        const QString &currency = "USD",
                        QWidget *parent = nullptr);

private slots:
  void onRent();
  void updatePrice();

private:
  CarInfo *car_;
  int customerId_;
  QString currentCurrency_;
  QDateTimeEdit *startDateEdit_;
  QDateTimeEdit *endDateEdit_;
  QLabel *totalPriceLabel_;
};
