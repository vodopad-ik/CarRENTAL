#pragma once

#include "db/CarInfo.h"

#include <QDate>
#include <QString>

class RentalCalculator {
public:
  struct DateNormalization {
    QDate start;
    QDate end;
    bool startClamped = false;
    bool endClamped = false;
    bool endAdjusted = false;
  };

  DateNormalization normalizeDates(const QDate &start, const QDate &end,
                                   const QDate &current,
                                   const QDate &max) const;
  int rentalDays(const QDate &start, const QDate &end) const;
  double totalBasePrice(const CarInfo &car, int days) const;
  QString formattedTotal(const CarInfo &car, int days,
                         const QString &currencyCode) const;

private:
  QString dayLabel(int days) const;
};

