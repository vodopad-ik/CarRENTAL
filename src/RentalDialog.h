#pragma once

#include "utils/RentalCalculator.h"

#include <QDateEdit>
#include <QDialog>
#include <QLabel>
#include <QString>

class CustomCalendarWidget;
class QVBoxLayout;

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
  void setupCarHeader(QVBoxLayout *layout);
  void setupCarSpecs(QVBoxLayout *layout);
  void setupDateForm(QVBoxLayout *layout);
  void setupPriceLabel(QVBoxLayout *layout);
  void setupButtons(QVBoxLayout *layout);
  void setupConnections();

  CarInfo *car_;
  int customerId_;
  QString currentCurrency_;
  QDateEdit *startDateEdit_;
  QDateEdit *endDateEdit_;
  QLabel *totalPriceLabel_;
  QLabel *specsLabel_ = nullptr;
  [[no_unique_address]] RentalCalculator calculator_;
};
