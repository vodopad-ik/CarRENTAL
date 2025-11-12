#pragma once

#include "db/CarInfo.h"

#include <QString>

class CarDetailsFormatter {
public:
  static QString tooltipText(const CarInfo &car);
  static QString name(const CarInfo &car);
};

