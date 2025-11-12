#include "CarsCatalogController.h"

#include "db/Database.h"

#include <QSqlQueryModel>
#include <memory>

void CarsCatalogController::setFilters(const CatalogFilters &filters) {
  filters_ = filters;
}

CatalogFilters CarsCatalogController::getFilters() const { return filters_; }

QList<CarInfo> CarsCatalogController::loadAvailable(int customerId) const {
  std::unique_ptr<QSqlQueryModel> model(
      Database::instance().getAvailableCars(customerId));
  return mapModelToCars(model.get());
}

QList<CarInfo> CarsCatalogController::loadBookmarked(int customerId) const {
  std::unique_ptr<QSqlQueryModel> model(
      Database::instance().getBookmarked(customerId));
  return mapModelToCars(model.get());
}

QList<CarInfo>
CarsCatalogController::mapModelToCars(QSqlQueryModel *model) const {
  QList<CarInfo> cars;
  if (!model)
    return cars;

  for (int i = 0; i < model->rowCount(); ++i) {
    CarInfo car;
    car.id = model->data(model->index(i, 0)).toInt();
    car.brand = model->data(model->index(i, 1)).toString();
    car.model = model->data(model->index(i, 2)).toString();
    car.year = model->data(model->index(i, 3)).toInt();
    car.pricePerDay = model->data(model->index(i, 4)).toDouble();
    car.description = model->data(model->index(i, 5)).toString();
    car.available = model->data(model->index(i, 6)).toBool();
    car.imagePath = model->data(model->index(i, 7)).toString();
    const int columnCount = model->columnCount();
    if (columnCount > 8)
      car.bookmarked = model->data(model->index(i, 8)).toInt() != 0;
    if (columnCount > 9)
      car.engineType = model->data(model->index(i, 9)).toString();
    if (columnCount > 10)
      car.engineCapacityL = model->data(model->index(i, 10)).toDouble();
    if (columnCount > 11)
      car.powerHp = model->data(model->index(i, 11)).toInt();
    if (columnCount > 12)
      car.seats = model->data(model->index(i, 12)).toInt();
    if (columnCount > 13)
      car.quantity = model->data(model->index(i, 13)).toInt();
    if (passesFilters(car))
      cars.append(car);
  }
  return cars;
}

bool CarsCatalogController::passesFilters(const CarInfo &car) const {
  const QString carText = QString("%1 %2 %3")
                              .arg(car.brand, car.model)
                              .arg(QString::number(car.year))
                              .toLower();

  if (!filters_.searchText.isEmpty() &&
      !carText.contains(filters_.searchText.toLower()))
    return false;

  if (!filters_.engineType.isEmpty() && filters_.engineType != "Любой" &&
      !car.engineType.isEmpty() && car.engineType != filters_.engineType)
    return false;

  if (filters_.seatsMin > 0 && car.seats < filters_.seatsMin)
    return false;

  if (filters_.powerMin > 0 && car.powerHp < filters_.powerMin)
    return false;

  if (filters_.capacityMin > 0.0 && car.engineCapacityL < filters_.capacityMin)
    return false;

  return true;
}
