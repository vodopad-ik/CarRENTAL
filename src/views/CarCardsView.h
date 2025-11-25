#pragma once

#include "db/CarInfo.h"

#include <QGridLayout>
#include <QList>
#include <QObject>
#include <QString>

#include <type_traits>

#include "CarCardWidget.h"

class CarCardsView : public QObject {
  Q_OBJECT

public:
  explicit CarCardsView(QWidget *container, int columns = 3);

  void setCurrency(const QString &currency);
  template <typename Binder>
  void showCars(const QList<CarInfo> &cars, Binder &&binder = nullptr);
  void clear();

  template <typename Visitor> void forEachCard(Visitor &&visitor) const;

private:
  QWidget *container_;
  QGridLayout *layout_ = nullptr;
  int columns_;
  QString currentCurrency_;
  QList<CarCardWidget *> cards_;
};

template <typename Binder>
void CarCardsView::showCars(const QList<CarInfo> &cars, Binder &&binder) {
  clear();

  int row = 0; 
  int col = 0;
  for (const auto &car : cars) {
    auto *card = new CarCardWidget(car, currentCurrency_, container_);
    if constexpr (!std::is_same_v<std::nullptr_t, std::decay_t<Binder>>) {
      binder(card);
    }
    layout_->addWidget(card, row, col, 1, 1, Qt::AlignLeft | Qt::AlignTop);
    cards_.append(card);

    ++col;
    if (col >= columns_) {
      col = 0;
      ++row;
    }
  }
}

template <typename Visitor>
void CarCardsView::forEachCard(Visitor &&visitor) const {
  for (auto *card : cards_) {
    visitor(card);
  }
}
