#include "CarSeeder.h"

#include <QSqlQuery>
#include <QStringList>
#include <QtGlobal>

void CarSeeder::seedIfEmpty(QSqlDatabase &db) {
  if (hasCars(db))
    return;

  int qtyCounter = 1;

  insertCar(db, "BMW", "3 Series", 2024, 10.8, qtyCounter++ % 10 + 1,
            "Спортивный седан премиум класса",
            ":/images/cars/bmw_3_series.jpg");
  insertCar(db, "BMW", "5 Series", 2024, 13.75, qtyCounter++ % 10 + 1,
            "Роскошный бизнес-седан", ":/images/cars/bmw_5_series.jpg");
  insertCar(db, "BMW", "X5", 2023, 16.69, qtyCounter++ % 10 + 1,
            "Премиум внедорожник", ":/images/cars/bmw_x5.jpg");
  insertCar(db, "Mercedes-Benz", "C-Class", 2023, 11.78, qtyCounter++ % 10 + 1,
            "Элегантность и комфорт", ":/images/cars/mercedes_c_class.jpg");
  insertCar(db, "Mercedes-Benz", "E-Class", 2024, 14.73, qtyCounter++ % 10 + 1,
            "Бизнес-класс высшего уровня",
            ":/images/cars/mercedes_e_class.jpeg");
  insertCar(db, "Mercedes-Benz", "GLE", 2023, 17.67, qtyCounter++ % 10 + 1,
            "Премиум кроссовер", ":/images/cars/mercedes_gle.jpeg");
  insertCar(db, "Audi", "A4", 2024, 9.82, qtyCounter++ % 10 + 1,
            "Динамика и стиль", ":/images/cars/audi_a4.jpg");
  insertCar(db, "Audi", "A6", 2024, 12.76, qtyCounter++ % 10 + 1,
            "Премиум седан", ":/images/cars/audi_a6.jpg");
  insertCar(db, "Audi", "Q5", 2023, 14.14, qtyCounter++ % 10 + 1,
            "Спортивный кроссовер", ":/images/cars/audi_q5.jpeg");
  insertCar(db, "Lexus", "ES", 2024, 12.17, qtyCounter++ % 10 + 1,
            "Японская роскошь", ":/images/cars/lexus_es.jpeg");
  insertCar(db, "Lexus", "RX", 2023, 15.32, qtyCounter++ % 10 + 1,
            "Премиум внедорожник", ":/images/cars/lexus_rx.jpeg");

  insertCar(db, "Toyota", "Camry", 2023, 5.89, qtyCounter++ % 10 + 1,
            "Комфортный седан для длительных поездок",
            ":/images/cars/toyota_camry.jpg");
  insertCar(db, "Toyota", "RAV4", 2024, 7.85, qtyCounter++ % 10 + 1,
            "Надежный кроссовер", ":/images/cars/toyota_rav4.jpg");
  insertCar(db, "Toyota", "Corolla", 2023, 4.91, qtyCounter++ % 10 + 1,
            "Экономичный и практичный", ":/images/cars/toyota_corolla.jpg");
  insertCar(db, "Honda", "Accord", 2023, 6.28, qtyCounter++ % 10 + 1,
            "Проверенная надежность", ":/images/cars/honda_accord.jpeg");
  insertCar(db, "Honda", "CR-V", 2024, 7.46, qtyCounter++ % 10 + 1,
            "Компактный внедорожник", ":/images/cars/honda_cr_v.jpg");
  insertCar(db, "Honda", "Civic", 2023, 5.5, qtyCounter++ % 10 + 1,
            "Спортивный седан", ":/images/cars/honda_civic.jpg");
  insertCar(db, "Volkswagen", "Passat", 2023, 6.87, qtyCounter++ % 10 + 1,
            "Надежный семейный автомобиль", ":/images/cars/vw_passat.jpg");
  insertCar(db, "Volkswagen", "Tiguan", 2024, 8.25, qtyCounter++ % 10 + 1,
            "Семейный кроссовер", ":/images/cars/vw_tiguan.jpeg");
  insertCar(db, "Volkswagen", "Golf", 2023, 5.89, qtyCounter++ % 10 + 1,
            "Компактный хетчбек", ":/images/cars/vw_golf.jpeg");
  insertCar(db, "Ford", "Focus", 2023, 5.5, qtyCounter++ % 10 + 1,
            "Компактный и экономичный", ":/images/cars/ford_focus.jpeg");
  insertCar(db, "Ford", "Escape", 2024, 7.07, qtyCounter++ % 10 + 1,
            "Городской кроссовер", ":/images/cars/ford_escape.jpg");
  insertCar(db, "Ford", "Mustang", 2023, 12.76, qtyCounter++ % 10 + 1,
            "Легендарный спорткар", ":/images/cars/ford_mustang.jpg");

  insertCar(db, "Hyundai", "Elantra", 2024, 5.69, qtyCounter++ % 10 + 1,
            "Современный дизайн, отличная цена",
            ":/images/cars/hyundai_elantra.jpg");
  insertCar(db, "Hyundai", "Tucson", 2023, 6.68, qtyCounter++ % 10 + 1,
            "Практичный кроссовер", ":/images/cars/hyundai_tucson.jpg");
  insertCar(db, "Hyundai", "Sonata", 2024, 6.09, qtyCounter++ % 10 + 1,
            "Стильный седан", ":/images/cars/hyundai_sonata.jpeg");
  insertCar(db, "Kia", "Optima", 2023, 5.89, qtyCounter++ % 10 + 1,
            "Комфортный седан", ":/images/cars/kia_optima.jpeg");
  insertCar(db, "Kia", "Sportage", 2024, 6.87, qtyCounter++ % 10 + 1,
            "Универсальный кроссовер", ":/images/cars/kia_sportage.jpeg");
  insertCar(db, "Kia", "Rio", 2023, 4.71, qtyCounter++ % 10 + 1,
            "Экономичный городской автомобиль", ":/images/cars/kia_rio.jpeg");
  insertCar(db, "Nissan", "Altima", 2024, 6.28, qtyCounter++ % 10 + 1,
            "Надежный седан", ":/images/cars/nissan_altima.jpeg");
  insertCar(db, "Nissan", "Rogue", 2023, 7.27, qtyCounter++ % 10 + 1,
            "Популярный кроссовер", ":/images/cars/nissan_rogue.jpeg");
  insertCar(db, "Nissan", "Sentra", 2024, 5.11, qtyCounter++ % 10 + 1,
            "Компактный седан", ":/images/cars/nissan_sentra.jpeg");
  insertCar(db, "Mazda", "CX-5", 2023, 7.66, qtyCounter++ % 10 + 1,
            "Спортивный кроссовер", ":/images/cars/mazda_cx5.jpeg");
  insertCar(db, "Mazda", "Mazda6", 2024, 6.68, qtyCounter++ % 10 + 1,
            "Элегантный седан", ":/images/cars/mazda_mazda6.jpeg");
  insertCar(db, "Mazda", "CX-3", 2023, 6.28, qtyCounter++ % 10 + 1,
            "Компактный кроссовер", ":/images/cars/mazda_cx3.jpeg");

  insertCar(db, "Porsche", "911", 2024, 29.45, qtyCounter++ % 10 + 1,
            "Икона спортивных автомобилей", ":/images/cars/porsche_911.jpeg");
  insertCar(db, "Porsche", "Cayenne", 2023, 23.56, qtyCounter++ % 10 + 1,
            "Роскошный внедорожник", ":/images/cars/porsche_cayenne.jpeg");
  insertCar(db, "Tesla", "Model 3", 2024, 15.71, qtyCounter++ % 10 + 1,
            "Электрический седан", ":/images/cars/tesla_model3.jpeg");
  insertCar(db, "Tesla", "Model Y", 2024, 18.65, qtyCounter++ % 10 + 1,
            "Электрический кроссовер", ":/images/cars/tesla_modely.jpg");
  insertCar(db, "Tesla", "Model S", 2024, 27.49, qtyCounter++ % 10 + 1,
            "Премиум электромобиль", ":/images/cars/tesla_models.jpeg");
  insertCar(db, "Jaguar", "XF", 2023, 15.32, qtyCounter++ % 10 + 1,
            "Британская роскошь", ":/images/cars/jaguar_xf.jpeg");
  insertCar(db, "Jaguar", "F-Pace", 2024, 16.69, qtyCounter++ % 10 + 1,
            "Премиум внедорожник", ":/images/cars/jaguar_fpace.jpeg");
  insertCar(db, "Range Rover", "Evoque", 2023, 19.64, qtyCounter++ % 10 + 1,
            "Компактный премиум внедорожник",
            ":/images/cars/rangerover_evoque.jpeg");
  insertCar(db, "Range Rover", "Sport", 2024, 25.53, qtyCounter++ % 10 + 1,
            "Спортивный премиум внедорожник",
            ":/images/cars/rangerover_sport.jpeg");
  insertCar(db, "Volvo", "XC60", 2023, 11.39, qtyCounter++ % 10 + 1,
            "Безопасный кроссовер", ":/images/cars/volvo_xc60.jpeg");
  insertCar(db, "Volvo", "S90", 2024, 12.17, qtyCounter++ % 10 + 1,
            "Премиум седан", ":/images/cars/volvo_s90.jpeg");
  insertCar(db, "Genesis", "G80", 2024, 13.35, qtyCounter++ % 10 + 1,
            "Роскошный седан", ":/images/cars/genesis_g80.jpeg");
  insertCar(db, "Genesis", "GV70", 2023, 14.14, qtyCounter++ % 10 + 1,
            "Премиум кроссовер", ":/images/cars/genesis_gv70.jpeg");
  insertCar(db, "Infiniti", "Q50", 2024, 10.8, qtyCounter++ % 10 + 1,
            "Спортивный премиум седан", ":/images/cars/infiniti_q50.jpeg");
  insertCar(db, "Infiniti", "QX50", 2023, 11.78, qtyCounter++ % 10 + 1,
            "Премиум кроссовер", "");

  insertCar(db, "Chevrolet", "Camaro", 2024, 14.73, qtyCounter++ % 10 + 1,
            "Американский мускул-кар", ":/images/cars/chevrolet_camaro.jpeg");
  insertCar(db, "Chevrolet", "Corvette", 2024, 24.55, qtyCounter++ % 10 + 1,
            "Легендарный спорткар", ":/images/cars/chevrolet_corvette.jpeg");
  insertCar(db, "Dodge", "Challenger", 2023, 13.75, qtyCounter++ % 10 + 1,
            "Мощный спорткар", ":/images/cars/dodge_challenger.jpg");
  insertCar(db, "Subaru", "WRX", 2024, 9.82, qtyCounter++ % 10 + 1,
            "Спортивный седан с полным приводом",
            ":/images/cars/subaru_wrx.jpeg");
  insertCar(db, "Subaru", "Outback", 2023, 8.25, qtyCounter++ % 10 + 1,
            "Универсал для путешествий", ":/images/cars/subaru_outback.jpeg");
  insertCar(db, "Mitsubishi", "Outlander", 2024, 7.07, qtyCounter++ % 10 + 1,
            "Семейный внедорожник", ":/images/cars/mitsubishi_outlander.jpg");
}

void CarSeeder::populateSpecs(QSqlDatabase &db) {
  struct Spec {
    const char *brand;
    const char *model;
    const char *type;
    double cap;
    int hp;
    int seats;
  };

  const Spec specs[] = {{"Audi", "A4", "ICE", 2.0, 190, 5},
                        {"Audi", "A6", "ICE", 2.0, 204, 5},
                        {"Audi", "Q5", "ICE", 2.0, 249, 5},
                        {"BMW", "3 Series", "Hybrid", 2.0, 292, 5},
                        {"BMW", "5 Series", "Hybrid", 2.0, 308, 5},
                        {"BMW", "X5", "Hybrid", 3.0, 340, 5},
                        {"Tesla", "Model 3", "EV", 0.0, 283, 5},
                        {"Tesla", "Model S", "EV", 0.0, 670, 5},
                        {"Toyota", "Camry", "ICE", 2.5, 206, 5},
                        {"Toyota", "Corolla", "ICE", 1.6, 132, 5},
                        {"Toyota", "RAV4", "Hybrid", 2.5, 222, 5},
                        {"Mercedes", "C-Class", "ICE", 1.5, 204, 5},
                        {"Mercedes", "E-Class", "ICE", 2.0, 258, 5},
                        {"Volkswagen", "Golf", "ICE", 1.4, 150, 5},
                        {"Volkswagen", "Passat", "ICE", 2.0, 190, 5}};

  for (const auto &s : specs) {
    QSqlQuery up(db);
    up.prepare("UPDATE cars SET engine_type=?, engine_capacity=?, power_hp=?, "
               "seats=? WHERE brand=? AND model=?");
    up.addBindValue(QString::fromUtf8(s.type));
    up.addBindValue(s.cap);
    up.addBindValue(s.hp);
    up.addBindValue(s.seats);
    up.addBindValue(QString::fromUtf8(s.brand));
    up.addBindValue(QString::fromUtf8(s.model));
    up.exec();
  }

  QSqlQuery sel(db);
  sel.exec("SELECT id, brand, model FROM cars WHERE engine_type IS NULL OR "
           "engine_type='' OR seats=0");
  const QStringList suvKeys = {
      "Q",       "X",     "RX",       "GL", "Tiguan", "RAV", "Outlander",
      "Cayenne", "Rogue", "Sportage", "CX", "GV",     "QX",  "XC"};

  while (sel.next()) {
    int id = sel.value(0).toInt();
    QString brand = sel.value(1).toString();
    QString model = sel.value(2).toString();

    QString type = "ICE";
    double cap = 2.0;
    int hp = 170;
    int seats = 5;

    if (brand.contains("Tesla", Qt::CaseInsensitive)) {
      type = "EV";
      cap = 0.0;
      hp = model.contains("S", Qt::CaseInsensitive) ? 670 : 283;
    } else if (brand.contains("Toyota", Qt::CaseInsensitive) &&
               model.contains("RAV", Qt::CaseInsensitive)) {
      type = "Hybrid";
      cap = 2.5;
      hp = 222;
    } else if (brand.contains("BMW", Qt::CaseInsensitive) &&
               (model.contains("5", Qt::CaseInsensitive) ||
                model.contains("X5", Qt::CaseInsensitive))) {
      type = "Hybrid";
      cap = model.contains("X5") ? 3.0 : 2.0;
      hp = model.contains("X5") ? 340 : 308;
    } else if (brand.contains("Mercedes", Qt::CaseInsensitive) &&
               model.contains("E", Qt::CaseInsensitive)) {
      type = "ICE";
      cap = 2.0;
      hp = 258;
    } else {
      bool isSUV = false;
      for (const auto &k : suvKeys) {
        if (model.contains(k, Qt::CaseInsensitive)) {
          isSUV = true;
          break;
        }
      }
      if (isSUV) {
        cap = 2.0;
        hp = 180;
      } else {
        cap = 1.6;
        hp = 130;
      }
    }

    QSqlQuery up(db);
    up.prepare("UPDATE cars SET engine_type=?, engine_capacity=?, power_hp=?, "
               "seats=? WHERE id=?");
    up.addBindValue(type);
    up.addBindValue(cap);
    up.addBindValue(hp);
    up.addBindValue(seats);
    up.addBindValue(id);
    up.exec();
  }
}

bool CarSeeder::hasCars(QSqlDatabase &db) const {
  QSqlQuery q(db);
  q.exec("SELECT COUNT(*) FROM cars");
  if (q.next())
    return q.value(0).toInt() > 0;
  return false;
}

bool CarSeeder::insertCar(QSqlDatabase &db, const QString &brand,
                          const QString &model, int year, double pricePerDay,
                          int quantity, const QString &description,
                          const QString &imagePath) {
  QSqlQuery query(db);
  query.prepare(
      "INSERT INTO cars (brand, model, year, price_per_day, quantity, "
      "description, image_path) VALUES (?, ?, ?, ?, ?, ?, ?)");
  query.addBindValue(brand);
  query.addBindValue(model);
  query.addBindValue(year);
  query.addBindValue(pricePerDay);
  query.addBindValue(qBound(1, quantity, 10));
  query.addBindValue(description);
  query.addBindValue(imagePath);
  return query.exec();
}
