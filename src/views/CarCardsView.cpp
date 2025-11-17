#include "CarCardsView.h"

#include "CarCardWidget.h"

#include <QLayoutItem>

CarCardsView::CarCardsView(QWidget *container, int columns)
    : QObject(container), container_(container), columns_(columns) {
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
  forEachCard([currency](CarCardWidget *card) { card->updateCurrency(currency); });
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
