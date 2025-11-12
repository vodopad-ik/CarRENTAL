#pragma once

#include "db/CarInfo.h"

#include <QGridLayout>
#include <QList>
#include <QObject>
#include <QString>

#include <functional>

class CarCardWidget;

class CarCardsView : public QObject {
  Q_OBJECT

public:
  CarCardsView(QWidget *container, int columns = 3);

  void setCurrency(const QString &currency);
  void showCars(const QList<CarInfo> &cars,
                const std::function<void(CarCardWidget *)> &binder = nullptr);
  void clear();

  void forEachCard(const std::function<void(CarCardWidget *)> &visitor) const;

private:
  QWidget *container_;
  QGridLayout *layout_;
  int columns_;
  QString currentCurrency_;
  QList<CarCardWidget *> cards_;
};

