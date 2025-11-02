#pragma once

#include <QDateEdit>
#include <QDialog>
#include <QLabel>
#include <QString>

class CustomCalendarWidget;

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
  QDateEdit *startDateEdit_;
  QDateEdit *endDateEdit_;
  QLabel *totalPriceLabel_;
  QLabel *specsLabel_ = nullptr;
};
