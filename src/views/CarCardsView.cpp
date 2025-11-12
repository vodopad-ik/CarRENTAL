#include "CarCardsView.h"

#include "CarCardWidget.h"

#include <QLayoutItem>

CarCardsView::CarCardsView(QWidget *container, int columns)
    : QObject(container), container_(container), layout_(nullptr),
      columns_(columns) {
  layout_ = qobject_cast<QGridLayout *>(container_->layout());
  if (!layout_) {
    layout_ = new QGridLayout(container_);
    layout_->setSpacing(20);
    layout_->setContentsMargins(20, 20, 20, 20);
  }

  for (int i = 0; i < columns_; ++i) {
    layout_->setColumnStretch(i, 0);
    layout_->setColumnMinimumWidth(i, 320);
  }
}

void CarCardsView::setCurrency(const QString &currency) {
  currentCurrency_ = currency;
  forEachCard([&](CarCardWidget *card) { card->updateCurrency(currency); });
}

void CarCardsView::showCars(
    const QList<CarInfo> &cars,
    const std::function<void(CarCardWidget *)> &binder) {
  clear();

  int row = 0;
  int col = 0;
  for (const auto &car : cars) {
    auto *card = new CarCardWidget(car, currentCurrency_, container_);
    if (binder)
      binder(card);
    layout_->addWidget(card, row, col, 1, 1, Qt::AlignLeft | Qt::AlignTop);
    cards_.append(card);

    ++col;
    if (col >= columns_) {
      col = 0;
      ++row;
    }
  }
}

void CarCardsView::clear() {
  while (QLayoutItem *item = layout_->takeAt(0)) {
    if (auto *widget = item->widget()) {
      widget->deleteLater();
    }
    delete item;
  }
  cards_.clear();
}

void CarCardsView::forEachCard(
    const std::function<void(CarCardWidget *)> &visitor) const {
  for (auto *card : cards_) {
    visitor(card);
  }
}
