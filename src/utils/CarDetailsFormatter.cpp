#include "CarDetailsFormatter.h"

QString CarDetailsFormatter::tooltipText(const CarInfo &car) {
  QString details;

  if (!car.engineType.isEmpty())
    details += QString("Тип двигателя: %1\n").arg(car.engineType);
  if (car.engineCapacityL > 0.0)
    details += QString("Объем: %1 л\n")
                    .arg(QString::number(car.engineCapacityL, 'f', 1));
  if (car.powerHp > 0)
    details += QString("Мощность: %1 л.с.\n").arg(car.powerHp);
  if (car.seats > 0)
    details += QString("Число мест: %1").arg(car.seats);

  if (details.isEmpty() && !car.description.isEmpty())
    details = car.description;

  return details.trimmed();
}

QString CarDetailsFormatter::name(const CarInfo &car) {
  return QString("%1 %2 (%3)")
      .arg(car.brand, car.model, QString::number(car.year));
}

