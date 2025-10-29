#pragma once

#include <QLabel>
#include <QPushButton>
#include <QWidget>

struct CarInfo;

class CarCardWidget : public QWidget {
  Q_OBJECT

public:
  explicit CarCardWidget(const CarInfo &car, const QString &currency = "USD",
                         QWidget *parent = nullptr);
  void updateCurrency(const QString &currency);

signals:
  void rentClicked(int carId);

private:
  void updatePriceDisplay();

  CarInfo *carData_;
  QString currentCurrency_;
  QLabel *imageLabel_;
  QLabel *nameLabel_;
  QLabel *priceLabel_;
  QLabel *descLabel_;
  QPushButton *rentBtn_;
};
