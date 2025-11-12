#include "RentalCalculator.h"

#include "CurrencyConverter.h"

RentalCalculator::DateNormalization
RentalCalculator::normalizeDates(const QDate &start, const QDate &end,
                                 const QDate &current, const QDate &max) const {
  DateNormalization result{start, end, false, false, false};

  if (result.start > max) {
    result.start = current;
    result.startClamped = true;
  }

  if (result.end > max) {
    result.end = max;
    result.endClamped = true;
  }

  if (result.end <= result.start) {
    result.end = result.start.addDays(1);
    result.endAdjusted = true;
  }

  return result;
}

int RentalCalculator::rentalDays(const QDate &start, const QDate &end) const {
  int days = start.daysTo(end);
  return days <= 0 ? 1 : days;
}

double RentalCalculator::totalBasePrice(const CarInfo &car, int days) const {
  return days * car.pricePerDay;
}

QString RentalCalculator::formattedTotal(const CarInfo &car, int days,
                                         const QString &currencyCode) const {
  auto &converter = CurrencyConverter::instance();
  const auto currency = CurrencyConverter::fromString(currencyCode);
  const double converted = converter.fromBase(car.pricePerDay, currency) * days;
  const QString symbol = converter.symbol(currency);
  return QString("Итого: %1 %2 (%3 %4)")
      .arg(converted, 0, 'f', 2)
      .arg(symbol)
      .arg(days)
      .arg(dayLabel(days));
}

QString RentalCalculator::dayLabel(int days) const {
  if (days == 1)
    return "день";
  if (days >= 2 && days <= 4)
    return "дня";
  return "дней";
}

