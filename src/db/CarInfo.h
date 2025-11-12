#pragma once

#include <QString>

struct CarInfo {
  int id;
  QString brand;
  QString model;
  int year;
  double pricePerDay;
  int quantity = 1;
  QString description;
  bool available;
  QString imagePath;
  bool bookmarked = false;
  QString engineType;
  double engineCapacityL = 0.0;
  int powerHp = 0;
  int seats = 0;
};

