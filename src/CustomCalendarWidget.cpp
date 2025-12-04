#include "CustomCalendarWidget.h"
#include "db/Database.h"

#include <QDate>
#include <QShowEvent>
#include <QTextCharFormat>

CustomCalendarWidget::CustomCalendarWidget(int carId, QWidget *parent)
    : QCalendarWidget(parent), carId_(carId) {}

void CustomCalendarWidget::showEvent(QShowEvent *event) {
  QCalendarWidget::showEvent(event);

  QTextCharFormat availableFormat;
  availableFormat.setForeground(QBrush(QColor(255, 255, 255)));
  availableFormat.setBackground(QBrush(QColor(76, 175, 80)));

  QTextCharFormat bookedFormat;
  bookedFormat.setForeground(QBrush(QColor(150, 150, 150)));
  bookedFormat.setBackground(QBrush(QColor(220, 220, 220)));

  QDate today = QDate::currentDate();
  for (int i = 0; i < 365; ++i) {
    QDate date = today.addDays(i);
    if (isDateBooked(date)) {
      setDateTextFormat(date, bookedFormat);
    } else {
      setDateTextFormat(date, availableFormat);
    }
  }
}

bool CustomCalendarWidget::isDateBooked(const QDate &date) const {
  if (carId_ <= 0)
    return false;

  int availableQty =
      Database::instance().getAvailableQuantity(carId_, date, date);
  return availableQty <= 0;
}
