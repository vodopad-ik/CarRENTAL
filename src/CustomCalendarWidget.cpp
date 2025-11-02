#include "CustomCalendarWidget.h"
#include "db/Database.h"

#include <QDate>
#include <QEvent>
#include <QMouseEvent>
#include <QShowEvent>
#include <QTextCharFormat>
#include <QToolTip>

CustomCalendarWidget::CustomCalendarWidget(int carId, QWidget *parent)
    : QCalendarWidget(parent), carId_(carId) {}

void CustomCalendarWidget::setCarId(int carId) { carId_ = carId; }

void CustomCalendarWidget::showEvent(QShowEvent *event) {
  QCalendarWidget::showEvent(event);

  // Обновляем стилизацию дат при показе календаря
  // Светлый цвет для доступных дат (можно забронировать)
  QTextCharFormat availableFormat;
  availableFormat.setForeground(
      QBrush(QColor(255, 255, 255))); // Белый цвет текста
  availableFormat.setBackground(
      QBrush(QColor(76, 175, 80))); // Светло-зеленый фон

  // Тусклый цвет для недоступных дат (нельзя забронировать)
  QTextCharFormat bookedFormat;
  bookedFormat.setForeground(
      QBrush(QColor(150, 150, 150))); // Серый цвет текста
  bookedFormat.setBackground(QBrush(QColor(220, 220, 220))); // Тускло-серый фон

  // Проверяем даты на следующий год (365 дней)
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

  // Вычисляем дату по позиции курсора
  QPoint pos = event->pos();

  // Проверяем все даты текущего месяца
  QDate monthStart = selectedDate();
  if (monthStart.isValid()) {
    monthStart = QDate(monthStart.year(), monthStart.month(), 1);
    QDate monthEnd = monthStart.addMonths(1).addDays(-1);

    for (QDate d = monthStart; d <= monthEnd; d = d.addDays(1)) {
      // Приблизительная проверка позиции
      // QCalendarWidget не предоставляет точный метод dateAt, поэтому
      // используем простую логику
      int dayOfWeek = d.dayOfWeek();
      int week = (d.day() - 1) / 7;
      int headerHeight = 30; // Примерная высота заголовка
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

  // Проверяем доступность на конкретную дату (как однодневную аренду)
  int availableQty =
      Database::instance().getAvailableQuantity(carId_, date, date);
  return availableQty <= 0;
}
