#pragma once

#include "db/CarInfo.h"

#include <QList>
#include <QString>

struct CatalogFilters {
  QString searchText;
  QString engineType;
  int seatsMin = 0;
  int powerMin = 0;
  double capacityMin = 0.0;
};

class CarsCatalogController {
public:
  void setFilters(const CatalogFilters &filters);
  CatalogFilters getFilters() const;

  QList<CarInfo> loadAvailable(int customerId) const;
  QList<CarInfo> loadBookmarked(int customerId) const;

private:
  QList<CarInfo> mapModelToCars(QSqlQueryModel *model) const;
  bool passesFilters(const CarInfo &car) const;

  CatalogFilters filters_;
};
