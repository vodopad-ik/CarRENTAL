#include "CustomCalendarWidget.h"
#include "db/Database.h"

#include <QDate>
#include <QMouseEvent>
#include <QShowEvent>
#include <QTextCharFormat>
#include <QToolTip>

CustomCalendarWidget::CustomCalendarWidget(int carId, QWidget *parent)
    : QCalendarWidget(parent), carId_(carId) {}

void CustomCalendarWidget::showEvent(QShowEvent *event) {
  QCalendarWidget::showEvent(event);



  QTextCharFormat availableFormat;
  availableFormat.setForeground(
      QBrush(QColor(255, 255, 255)));
  availableFormat.setBackground(
      QBrush(QColor(76, 175, 80)));


  QTextCharFormat bookedFormat;
  bookedFormat.setForeground(
      QBrush(QColor(150, 150, 150)));
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

void CustomCalendarWidget::mouseMoveEvent(QMouseEvent *event) {
  QCalendarWidget::mouseMoveEvent(event);


  QPoint pos = event->pos();


  QDate monthStart = selectedDate();
  if (monthStart.isValid()) {
    monthStart = QDate(monthStart.year(), monthStart.month(), 1);
    QDate monthEnd = monthStart.addMonths(1).addDays(-1);

    for (QDate d = monthStart; d <= monthEnd; d = d.addDays(1)) {



      int dayOfWeek = d.dayOfWeek();
      int week = (d.day() - 1) / 7;
      int headerHeight = 30;
      int cellHeight = (height() - headerHeight) / 6;
      int cellWidth = width() / 7;

      QRect cellRect(dayOfWeek * cellWidth, headerHeight + week * cellHeight,
                     cellWidth, cellHeight);

      if (cellRect.contains(pos) && d >= QDate::currentDate() &&
          isDateBooked(d)) {
        QToolTip::showText(
            event->globalPosition().toPoint(),
            "Автомобиль забронирован другим пользователем в этот день");
        return;
      }
    }
  }

  QToolTip::hideText();
}

bool CustomCalendarWidget::isDateBooked(const QDate &date) const {
  if (carId_ <= 0)
    return false;


  int availableQty =
      Database::instance().getAvailableQuantity(carId_, date, date);
  return availableQty <= 0;
}
