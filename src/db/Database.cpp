#include "Database.h"

#include <QDebug>
#include <QDir>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QVariant>

Database &Database::instance() {
  static Database inst;
  return inst;
}

Database::Database() {}

bool Database::initialize() {
  if (!QSqlDatabase::contains("carrental")) {
    db_ = QSqlDatabase::addDatabase("QSQLITE", "carrental");
  } else {
    db_ = QSqlDatabase::database("carrental");
  }

  const QString dataDir = QDir::homePath() + "/.carrental";
  QDir().mkpath(dataDir);
  const QString dbPath = dataDir + "/app.db";
  db_.setDatabaseName(dbPath);

  if (!db_.open()) {
    return false;
  }

  if (!createSchema())
    return false;
  ensureImagePaths();
  // Populate default specs for known models (approximate real values)
  {
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
      QSqlQuery up(db_);
      up.prepare("UPDATE cars SET engine_type=?, engine_capacity=?, "
                 "power_hp=?, seats=? WHERE brand=? AND model=?");
      up.addBindValue(QString::fromUtf8(s.type));
      up.addBindValue(s.cap);
      up.addBindValue(s.hp);
      up.addBindValue(s.seats);
      up.addBindValue(QString::fromUtf8(s.brand));
      up.addBindValue(QString::fromUtf8(s.model));
      up.exec();
    }
  }
  // Heuristics: fill specs for the rest if still missing
  {
    QSqlQuery sel(db_);
    sel.exec("SELECT id, brand, model FROM cars WHERE engine_type IS NULL OR "
             "engine_type='' OR seats=0");
    const QStringList suvKeys = {"Q",      "X",        "RX",        "GL",
                                 "Tiguan", "RAV",      "Outlander", "Cayenne",
                                 "Rogue",  "Sportage", "CX",        "GV",
                                 "QX",     "XC",       "Cayenne",   "Cayenne"};
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
        // SUV heuristic
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
      QSqlQuery up(db_);
      up.prepare("UPDATE cars SET engine_type=?, engine_capacity=?, "
                 "power_hp=?, seats=? WHERE id=?");
      up.addBindValue(type);
      up.addBindValue(cap);
      up.addBindValue(hp);
      up.addBindValue(seats);
      up.addBindValue(id);
      up.exec();
    }
  }
  // Populate realistic short descriptions if missing
  {
    struct Desc {
      const char *brand;
      const char *model;
      const char *d;
    };
    const Desc descs[] = {
        {"Audi", "A4",
         "Динамичный седан с отличной управляемостью и комфортом."},
        {"Audi", "A6", "Премиум-седан бизнес‑класса с богатым оснащением."},
        {"Audi", "Q5",
         "Спортивный кроссовер с полным приводом и просторным салоном."},
        {"BMW", "3 Series",
         "Спортивный характер, точная рулёжка, отличный баланс."},
        {"BMW", "5 Series",
         "Комфортный бизнес‑седан, тихий и очень собранный."},
        {"BMW", "X5", "Большой семейный кроссовер, мощный и технологичный."},
        {"Tesla", "Model 3", "Электроседан с быстрым разгоном и автопилотом."},
        {"Tesla", "Model S",
         "Флагманский электроседан с большим запасом хода."},
        {"Toyota", "Camry", "Надёжный и комфортный седан для дальних поездок."},
        {"Toyota", "Corolla",
         "Экономичный городской седан с мягкой подвеской."},
        {"Toyota", "RAV4", "Практичный кроссовер с высоким клиренсом."},
        {"Mercedes", "C-Class", "Стильный седан с премиальным салоном."},
        {"Mercedes", "E-Class",
         "Комфорт высшего уровня, отличная шумоизоляция."},
        {"Volkswagen", "Golf",
         "Компактный хэтчбек, удобный в городе и на трассе."},
        {"Volkswagen", "Passat",
         "Просторный семейный седан с большим багажником."}};
    for (const auto &d : descs) {
      QSqlQuery up(db_);
      up.prepare("UPDATE cars SET description=? WHERE (description IS NULL OR "
                 "description='') AND brand=? AND model=?");
      up.addBindValue(QString::fromUtf8(d.d));
      up.addBindValue(QString::fromUtf8(d.brand));
      up.addBindValue(QString::fromUtf8(d.model));
      up.exec();
    }
  }
  setupCarsModel();
  return true;
}

bool Database::createSchema() {
  QSqlQuery q(db_);

  // Cars table (migrate to price_per_hour + image_path)
  if (!q.exec("CREATE TABLE IF NOT EXISTS cars ("
              "id INTEGER PRIMARY KEY AUTOINCREMENT,"
              "brand TEXT NOT NULL,"
              "model TEXT NOT NULL,"
              "year INTEGER NOT NULL,"
              "price_per_day REAL NOT NULL,"
              "quantity INTEGER DEFAULT 1,"
              "description TEXT,"
              "available INTEGER DEFAULT 1,"
              "image_path TEXT,"
              "bookmarked INTEGER DEFAULT 0,"
              "engine_type TEXT DEFAULT '',"
              "engine_capacity REAL DEFAULT 0,"
              "power_hp INTEGER DEFAULT 0,"
              "seats INTEGER DEFAULT 0"
              ")")) {
    return false;
  }

  // Bookmarks table - закладки пользователей
  if (!q.exec("CREATE TABLE IF NOT EXISTS bookmarks ("
              "id INTEGER PRIMARY KEY AUTOINCREMENT,"
              "customer_id INTEGER NOT NULL,"
              "car_id INTEGER NOT NULL,"
              "UNIQUE(customer_id, car_id),"
              "FOREIGN KEY(customer_id) REFERENCES customers(id),"
              "FOREIGN KEY(car_id) REFERENCES cars(id)"
              ")")) {
    return false;
  }

  // Schema migrations table - отслеживание выполненных миграций
  if (!q.exec("CREATE TABLE IF NOT EXISTS schema_migrations ("
              "migration_name TEXT PRIMARY KEY,"
              "executed_at INTEGER DEFAULT (strftime('%s', 'now'))"
              ")")) {
    return false;
  }

  // Backward compatibility and ensure extra columns
  if (q.exec("PRAGMA table_info(cars)")) {
    bool hasPricePerDay = false;
    bool hasPricePerHour = false;
    bool hasQuantity = false;
    bool hasImagePath = false;
    bool hasBookmarked = false;
    bool hasEngineType = false;
    bool hasEngineCapacity = false;
    bool hasPowerHp = false;
    bool hasSeats = false;
    while (q.next()) {
      const QString name = q.value(1).toString();
      if (name == "price_per_day")
        hasPricePerDay = true;
      if (name == "price_per_hour")
        hasPricePerHour = true;
      if (name == "quantity")
        hasQuantity = true;
      if (name == "image_path")
        hasImagePath = true;
      if (name == "bookmarked")
        hasBookmarked = true;
      if (name == "engine_type")
        hasEngineType = true;
      if (name == "engine_capacity")
        hasEngineCapacity = true;
      if (name == "power_hp")
        hasPowerHp = true;
      if (name == "seats")
        hasSeats = true;
    }
    // Миграция: если есть price_per_hour, конвертируем в price_per_day
    if (hasPricePerHour && !hasPricePerDay) {
      QSqlQuery alter(db_);
      alter.exec("ALTER TABLE cars ADD COLUMN price_per_day REAL");
      // Конвертируем цену за час в цену за день (умножаем на 24)
      alter.exec("UPDATE cars SET price_per_day = price_per_hour * 24.0 WHERE "
                 "price_per_day IS NULL");
    }
    // Если есть price_per_day, но значения старые (меньше 100), возможно это
    // еще цены за час Обновляем только если price_per_day < 100 (обычно цены за
    // день выше)
    // Это делается ТОЛЬКО один раз через таблицу миграций
    {
      QSqlQuery migCheck(db_);
      migCheck.prepare(
          "SELECT COUNT(*) FROM schema_migrations WHERE migration_name = ?");
      migCheck.addBindValue("price_convert_hour_to_day");
      migCheck.exec();
      bool alreadyExecuted = false;
      if (migCheck.next() && migCheck.value(0).toInt() > 0) {
        alreadyExecuted = true;
      }

      if (!alreadyExecuted) {
        QSqlQuery check(db_);
        if (check.exec("SELECT COUNT(*) FROM cars WHERE price_per_day > 0 AND "
                       "price_per_day < 100")) {
          if (check.next() && check.value(0).toInt() > 0) {
            QSqlQuery update(db_);
            update.exec("UPDATE cars SET price_per_day = price_per_day * 24.0 "
                        "WHERE price_per_day > 0 AND price_per_day < 100");
            // Отмечаем миграцию как выполненную
            QSqlQuery migInsert(db_);
            migInsert.prepare("INSERT OR IGNORE INTO schema_migrations "
                              "(migration_name) VALUES (?)");
            migInsert.addBindValue("price_convert_hour_to_day");
            migInsert.exec();
          }
        }
      }
    }
    if (!hasQuantity) {
      QSqlQuery addQty(db_);
      addQty.exec("ALTER TABLE cars ADD COLUMN quantity INTEGER DEFAULT 1");
      // Устанавливаем случайное количество от 1 до 10 для существующих машин
      addQty.exec("UPDATE cars SET quantity = (ABS(RANDOM()) % 10) + 1 WHERE "
                  "quantity IS NULL OR quantity = 0");
    }
    // Уменьшаем все цены на 10% (умножаем на 0.9) для существующих записей
    // Это делается ТОЛЬКО один раз через таблицу миграций
    {
      QSqlQuery migCheck(db_);
      migCheck.prepare(
          "SELECT COUNT(*) FROM schema_migrations WHERE migration_name = ?");
      migCheck.addBindValue("price_reduce_10_percent");
      migCheck.exec();
      bool alreadyExecuted = false;
      if (migCheck.next() && migCheck.value(0).toInt() > 0) {
        alreadyExecuted = true;
      }

      if (!alreadyExecuted) {
        QSqlQuery priceCheck(db_);
        priceCheck.exec("SELECT COUNT(*) FROM cars WHERE price_per_day > 100");
        if (priceCheck.next() && priceCheck.value(0).toInt() > 0) {
          // Если есть цены больше 100, значит они еще не уменьшены
          QSqlQuery priceUpdate(db_);
          priceUpdate.exec(
              "UPDATE cars SET price_per_day = price_per_day * 0.9 "
              "WHERE price_per_day > 0");
          // Отмечаем миграцию как выполненную
          QSqlQuery migInsert(db_);
          migInsert.prepare("INSERT OR IGNORE INTO schema_migrations "
                            "(migration_name) VALUES (?)");
          migInsert.addBindValue("price_reduce_10_percent");
          migInsert.exec();
        }
      }
    }
    // Уменьшаем все цены в 11 раз (делим на 11) для существующих записей
    // Это делается ТОЛЬКО один раз через таблицу миграций
    {
      QSqlQuery migCheck(db_);
      migCheck.prepare(
          "SELECT COUNT(*) FROM schema_migrations WHERE migration_name = ?");
      migCheck.addBindValue("price_reduce_11x");
      migCheck.exec();
      bool alreadyExecuted = false;
      if (migCheck.next() && migCheck.value(0).toInt() > 0) {
        alreadyExecuted = true;
      }

      if (!alreadyExecuted) {
        QSqlQuery priceCheck(db_);
        priceCheck.exec("SELECT COUNT(*) FROM cars WHERE price_per_day > 10");
        if (priceCheck.next() && priceCheck.value(0).toInt() > 0) {
          // Если есть цены больше 10, значит они еще не уменьшены в 11 раз
          QSqlQuery priceUpdate(db_);
          priceUpdate.exec(
              "UPDATE cars SET price_per_day = price_per_day / 11.0 "
              "WHERE price_per_day > 0");
          // Отмечаем миграцию как выполненную
          QSqlQuery migInsert(db_);
          migInsert.prepare("INSERT OR IGNORE INTO schema_migrations "
                            "(migration_name) VALUES (?)");
          migInsert.addBindValue("price_reduce_11x");
          migInsert.exec();
        }
      }
    }
    if (!hasImagePath) {
      QSqlQuery addImg(db_);
      addImg.exec("ALTER TABLE cars ADD COLUMN image_path TEXT");
    }
    if (!hasBookmarked) {
      QSqlQuery addB(db_);
      addB.exec("ALTER TABLE cars ADD COLUMN bookmarked INTEGER DEFAULT 0");
    }
    if (!hasEngineType) {
      QSqlQuery add(db_);
      add.exec("ALTER TABLE cars ADD COLUMN engine_type TEXT DEFAULT ''");
    }
    if (!hasEngineCapacity) {
      QSqlQuery add(db_);
      add.exec("ALTER TABLE cars ADD COLUMN engine_capacity REAL DEFAULT 0");
    }
    if (!hasPowerHp) {
      QSqlQuery add(db_);
      add.exec("ALTER TABLE cars ADD COLUMN power_hp INTEGER DEFAULT 0");
    }
    if (!hasSeats) {
      QSqlQuery add(db_);
      add.exec("ALTER TABLE cars ADD COLUMN seats INTEGER DEFAULT 0");
    }
  }

  // ensure columns in customers
  if (q.exec("PRAGMA table_info(customers)")) {
    bool hasPassword = false;
    while (q.next()) {
      if (q.value(1).toString() == "password_hash")
        hasPassword = true;
    }
    if (!hasPassword) {
      QSqlQuery addPwd(db_);
      addPwd.exec("ALTER TABLE customers ADD COLUMN password_hash TEXT");
    }
  }

  // Customers table
  if (!q.exec("CREATE TABLE IF NOT EXISTS customers ("
              "id INTEGER PRIMARY KEY AUTOINCREMENT,"
              "name TEXT NOT NULL,"
              "phone TEXT NOT NULL,"
              "email TEXT,"
              "password_hash TEXT"
              ")")) {
    return false;
  }
  // Ensure unique phone index
  {
    QSqlQuery uq(db_);
    uq.exec("CREATE UNIQUE INDEX IF NOT EXISTS idx_customers_phone_unique ON "
            "customers(phone)");
  }

  // Rentals table
  if (!q.exec("CREATE TABLE IF NOT EXISTS rentals ("
              "id INTEGER PRIMARY KEY AUTOINCREMENT,"
              "car_id INTEGER NOT NULL,"
              "customer_id INTEGER NOT NULL,"
              "start_date TEXT NOT NULL,"
              "end_date TEXT NOT NULL,"
              "total_price REAL NOT NULL,"
              "status TEXT DEFAULT 'active',"
              "FOREIGN KEY(car_id) REFERENCES cars(id),"
              "FOREIGN KEY(customer_id) REFERENCES customers(id)"
              ")")) {
    return false;
  }

  // Insert sample cars if empty (50 автомобилей)
  q.exec("SELECT COUNT(*) FROM cars");
  int count = 0;
  if (q.next()) {
    count = q.value(0).toInt();
  }

  if (count == 0) {
    // Premium класс - цены указаны за день (конвертированы из часовых цен * 24)
    // Генерируем случайное количество от 1 до 10 для каждой модели
    // Используем простой счетчик для генерации разных значений
    int qtyCounter = 1;
    insertCar("BMW", "3 Series", 2024, 10.8, qtyCounter++ % 10 + 1,
              "Спортивный седан премиум класса",
              ":/images/cars/bmw_3_series.svg");
    insertCar("BMW", "5 Series", 2024, 13.75, qtyCounter++ % 10 + 1,
              "Роскошный бизнес-седан", ":/images/cars/bmw_5_series.svg");
    insertCar("BMW", "X5", 2023, 16.69, qtyCounter++ % 10 + 1,
              "Премиум внедорожник", ":/images/cars/bmw_x5.svg");
    insertCar("Mercedes-Benz", "C-Class", 2023, 11.78, qtyCounter++ % 10 + 1,
              "Элегантность и комфорт", ":/images/cars/mercedes_c_class.svg");
    insertCar("Mercedes-Benz", "E-Class", 2024, 14.73, qtyCounter++ % 10 + 1,
              "Бизнес-класс высшего уровня",
              ":/images/cars/mercedes_e_class.svg");
    insertCar("Mercedes-Benz", "GLE", 2023, 17.67, qtyCounter++ % 10 + 1,
              "Премиум кроссовер", ":/images/cars/mercedes_gle.svg");
    insertCar("Audi", "A4", 2024, 9.82, qtyCounter++ % 10 + 1,
              "Динамика и стиль", ":/images/cars/audi_a4.svg");
    insertCar("Audi", "A6", 2024, 12.76, qtyCounter++ % 10 + 1, "Премиум седан",
              ":/images/cars/audi_a6.svg");
    insertCar("Audi", "Q5", 2023, 14.14, qtyCounter++ % 10 + 1,
              "Спортивный кроссовер", ":/images/cars/audi_q5.svg");
    insertCar("Lexus", "ES", 2024, 12.17, qtyCounter++ % 10 + 1,
              "Японская роскошь", ":/images/cars/lexus_es.svg");
    insertCar("Lexus", "RX", 2023, 15.32, qtyCounter++ % 10 + 1,
              "Премиум внедорожник", ":/images/cars/lexus_rx.svg");

    // Средний класс
    insertCar("Toyota", "Camry", 2023, 5.89, qtyCounter++ % 10 + 1,
              "Комфортный седан для длительных поездок",
              ":/images/cars/toyota_camry.svg");
    insertCar("Toyota", "RAV4", 2024, 7.85, qtyCounter++ % 10 + 1,
              "Надежный кроссовер", ":/images/cars/toyota_rav4.svg");
    insertCar("Toyota", "Corolla", 2023, 4.91, qtyCounter++ % 10 + 1,
              "Экономичный и практичный", ":/images/cars/toyota_corolla.svg");
    insertCar("Honda", "Accord", 2023, 6.28, qtyCounter++ % 10 + 1,
              "Проверенная надежность", ":/images/cars/honda_accord.svg");
    insertCar("Honda", "CR-V", 2024, 7.46, qtyCounter++ % 10 + 1,
              "Компактный внедорожник", ":/images/cars/honda_cr_v.svg");
    insertCar("Honda", "Civic", 2023, 5.5, qtyCounter++ % 10 + 1,
              "Спортивный седан", ":/images/cars/honda_civic.svg");
    insertCar("Volkswagen", "Passat", 2023, 6.87, qtyCounter++ % 10 + 1,
              "Надежный семейный автомобиль", ":/images/cars/vw_passat.svg");
    insertCar("Volkswagen", "Tiguan", 2024, 8.25, qtyCounter++ % 10 + 1,
              "Семейный кроссовер", ":/images/cars/vw_tiguan.svg");
    insertCar("Volkswagen", "Golf", 2023, 5.89, qtyCounter++ % 10 + 1,
              "Компактный хетчбек", ":/images/cars/vw_golf.svg");
    insertCar("Ford", "Focus", 2023, 5.5, qtyCounter++ % 10 + 1,
              "Компактный и экономичный", ":/images/cars/ford_focus.svg");
    insertCar("Ford", "Escape", 2024, 7.07, qtyCounter++ % 10 + 1,
              "Городской кроссовер", ":/images/cars/ford_escape.svg");
    insertCar("Ford", "Mustang", 2023, 12.76, qtyCounter++ % 10 + 1,
              "Легендарный спорткар", ":/images/cars/ford_mustang.svg");

    // Бюджетный класс
    insertCar("Hyundai", "Elantra", 2024, 5.69, qtyCounter++ % 10 + 1,
              "Современный дизайн, отличная цена",
              ":/images/cars/hyundai_elantra.svg");
    insertCar("Hyundai", "Tucson", 2023, 6.68, qtyCounter++ % 10 + 1,
              "Практичный кроссовер", ":/images/cars/hyundai_tucson.svg");
    insertCar("Hyundai", "Sonata", 2024, 6.09, qtyCounter++ % 10 + 1,
              "Стильный седан", ":/images/cars/hyundai_sonata.svg");
    insertCar("Kia", "Optima", 2023, 5.89, qtyCounter++ % 10 + 1,
              "Комфортный седан", ":/images/cars/kia_optima.svg");
    insertCar("Kia", "Sportage", 2024, 6.87, qtyCounter++ % 10 + 1,
              "Универсальный кроссовер", ":/images/cars/kia_sportage.svg");
    insertCar("Kia", "Rio", 2023, 4.71, qtyCounter++ % 10 + 1,
              "Экономичный городской автомобиль", ":/images/cars/kia_rio.svg");
    insertCar("Nissan", "Altima", 2024, 6.28, qtyCounter++ % 10 + 1,
              "Надежный седан", ":/images/cars/nissan_altima.svg");
    insertCar("Nissan", "Rogue", 2023, 7.27, qtyCounter++ % 10 + 1,
              "Популярный кроссовер", ":/images/cars/nissan_rogue.svg");
    insertCar("Nissan", "Sentra", 2024, 5.11, qtyCounter++ % 10 + 1,
              "Компактный седан", ":/images/cars/nissan_sentra.svg");
    insertCar("Mazda", "CX-5", 2023, 7.66, qtyCounter++ % 10 + 1,
              "Спортивный кроссовер", ":/images/cars/mazda_cx5.svg");
    insertCar("Mazda", "Mazda6", 2024, 6.68, qtyCounter++ % 10 + 1,
              "Элегантный седан", ":/images/cars/mazda_mazda6.svg");
    insertCar("Mazda", "CX-3", 2023, 6.28, qtyCounter++ % 10 + 1,
              "Компактный кроссовер", ":/images/cars/mazda_cx3.svg");

    // Люкс класс
    insertCar("Porsche", "911", 2024, 29.45, qtyCounter++ % 10 + 1,
              "Икона спортивных автомобилей", ":/images/cars/porsche_911.svg");
    insertCar("Porsche", "Cayenne", 2023, 23.56, qtyCounter++ % 10 + 1,
              "Роскошный внедорожник", ":/images/cars/porsche_cayenne.svg");
    insertCar("Tesla", "Model 3", 2024, 15.71, qtyCounter++ % 10 + 1,
              "Электрический седан", ":/images/cars/tesla_model3.svg");
    insertCar("Tesla", "Model Y", 2024, 18.65, qtyCounter++ % 10 + 1,
              "Электрический кроссовер", ":/images/cars/tesla_modely.svg");
    insertCar("Tesla", "Model S", 2024, 27.49, qtyCounter++ % 10 + 1,
              "Премиум электромобиль", ":/images/cars/tesla_models.svg");
    insertCar("Jaguar", "XF", 2023, 15.32, qtyCounter++ % 10 + 1,
              "Британская роскошь", ":/images/cars/jaguar_xf.svg");
    insertCar("Jaguar", "F-Pace", 2024, 16.69, qtyCounter++ % 10 + 1,
              "Премиум внедорожник", ":/images/cars/jaguar_fpace.svg");
    insertCar("Range Rover", "Evoque", 2023, 19.64, qtyCounter++ % 10 + 1,
              "Компактный премиум внедорожник",
              ":/images/cars/rangerover_evoque.svg");
    insertCar("Range Rover", "Sport", 2024, 25.53, qtyCounter++ % 10 + 1,
              "Спортивный премиум внедорожник",
              ":/images/cars/rangerover_sport.svg");
    insertCar("Volvo", "XC60", 2023, 11.39, qtyCounter++ % 10 + 1,
              "Безопасный кроссовер", ":/images/cars/volvo_xc60.svg");
    insertCar("Volvo", "S90", 2024, 12.17, qtyCounter++ % 10 + 1,
              "Премиум седан", ":/images/cars/volvo_s90.svg");
    insertCar("Genesis", "G80", 2024, 13.35, qtyCounter++ % 10 + 1,
              "Роскошный седан", ":/images/cars/genesis_g80.svg");
    insertCar("Genesis", "GV70", 2023, 14.14, qtyCounter++ % 10 + 1,
              "Премиум кроссовер", ":/images/cars/genesis_gv70.svg");
    insertCar("Infiniti", "Q50", 2024, 10.8, qtyCounter++ % 10 + 1,
              "Спортивный премиум седан", ":/images/cars/infiniti_q50.svg");
    insertCar("Infiniti", "QX50", 2023, 11.78, qtyCounter++ % 10 + 1,
              "Премиум кроссовер", "");

    // Спортивные
    insertCar("Chevrolet", "Camaro", 2024, 14.73, qtyCounter++ % 10 + 1,
              "Американский мускул-кар", ":/images/cars/chevrolet_camaro.svg");
    insertCar("Chevrolet", "Corvette", 2024, 24.55, qtyCounter++ % 10 + 1,
              "Легендарный спорткар", ":/images/cars/chevrolet_corvette.svg");
    insertCar("Dodge", "Challenger", 2023, 13.75, qtyCounter++ % 10 + 1,
              "Мощный спорткар", ":/images/cars/dodge_challenger.svg");
    insertCar("Subaru", "WRX", 2024, 9.82, qtyCounter++ % 10 + 1,
              "Спортивный седан с полным приводом",
              ":/images/cars/subaru_wrx.svg");
    insertCar("Subaru", "Outback", 2023, 8.25, qtyCounter++ % 10 + 1,
              "Универсал для путешествий", ":/images/cars/subaru_outback.svg");
    insertCar("Mitsubishi", "Outlander", 2024, 7.07, qtyCounter++ % 10 + 1,
              "Семейный внедорожник", ":/images/cars/mitsubishi_outlander.svg");
  } else if (count < 50) {
    // Если автомобилей меньше 50, дополняем до 50
    // Список всех 50 автомобилей (уже вставлены выше)
    // Эта логика может быть расширена для добавления недостающих
    qDebug() << "В базе данных" << count << "автомобилей, необходимо 50";
  }

  return true;
}

void Database::ensureImagePaths() {
  // Проставляем image_path для записей, где он пустой,
  // чтобы UI мог вычислять имя файла реального фото по ключу.
  struct Map {
    const char *brand;
    const char *model;
    const char *key;
  };
  static const Map maps[] = {
      {"BMW", "3 Series", "bmw_3_series"},
      {"BMW", "5 Series", "bmw_5_series"},
      {"BMW", "X5", "bmw_x5"},
      {"Mercedes-Benz", "C-Class", "mercedes_c_class"},
      {"Mercedes-Benz", "E-Class", "mercedes_e_class"},
      {"Mercedes-Benz", "GLE", "mercedes_gle"},
      {"Audi", "A4", "audi_a4"},
      {"Audi", "A6", "audi_a6"},
      {"Audi", "Q5", "audi_q5"},
      {"Lexus", "ES", "lexus_es"},
      {"Lexus", "RX", "lexus_rx"},
      {"Toyota", "Camry", "toyota_camry"},
      {"Toyota", "RAV4", "toyota_rav4"},
      {"Toyota", "Corolla", "toyota_corolla"},
      {"Honda", "Accord", "honda_accord"},
      {"Honda", "CR-V", "honda_cr_v"},
      {"Honda", "Civic", "honda_civic"},
      {"Volkswagen", "Passat", "vw_passat"},
      {"Volkswagen", "Tiguan", "vw_tiguan"},
      {"Volkswagen", "Golf", "vw_golf"},
      {"Ford", "Focus", "ford_focus"},
      {"Ford", "Escape", "ford_escape"},
      {"Ford", "Mustang", "ford_mustang"},
      {"Hyundai", "Elantra", "hyundai_elantra"},
      {"Hyundai", "Tucson", "hyundai_tucson"},
      {"Hyundai", "Sonata", "hyundai_sonata"},
      {"Kia", "Optima", "kia_optima"},
      {"Kia", "Sportage", "kia_sportage"},
      {"Kia", "Rio", "kia_rio"},
      {"Nissan", "Altima", "nissan_altima"},
      {"Nissan", "Rogue", "nissan_rogue"},
      {"Nissan", "Sentra", "nissan_sentra"},
      {"Mazda", "CX-5", "mazda_cx5"},
      {"Mazda", "Mazda6", "mazda_mazda6"},
      {"Mazda", "CX-3", "mazda_cx3"},
      {"Porsche", "911", "porsche_911"},
      {"Porsche", "Cayenne", "porsche_cayenne"},
      {"Tesla", "Model 3", "tesla_model3"},
      {"Tesla", "Model Y", "tesla_modely"},
      {"Tesla", "Model S", "tesla_models"},
      {"Jaguar", "XF", "jaguar_xf"},
      {"Jaguar", "F-Pace", "jaguar_fpace"},
      {"Range Rover", "Evoque", "rangerover_evoque"},
      {"Range Rover", "Sport", "rangerover_sport"},
      {"Volvo", "XC60", "volvo_xc60"},
      {"Volvo", "S90", "volvo_s90"},
      {"Genesis", "G80", "genesis_g80"},
      {"Genesis", "GV70", "genesis_gv70"},
      {"Infiniti", "Q50", "infiniti_q50"},
      {"Infiniti", "QX50", "infiniti_qx50"},
      {"Chevrolet", "Camaro", "chevrolet_camaro"},
      {"Chevrolet", "Corvette", "chevrolet_corvette"},
      {"Dodge", "Challenger", "dodge_challenger"},
      {"Subaru", "WRX", "subaru_wrx"},
      {"Subaru", "Outback", "subaru_outback"},
      {"Mitsubishi", "Outlander", "mitsubishi_outlander"},
  };

  for (const auto &m : maps) {
    QSqlQuery q(db_);
    q.prepare("UPDATE cars SET image_path = ? WHERE (image_path IS NULL OR "
              "image_path='' ) AND brand=? AND model=?");
    q.addBindValue(QString(":/images/cars/%1.svg").arg(m.key));
    q.addBindValue(QString::fromUtf8(m.brand));
    q.addBindValue(QString::fromUtf8(m.model));
    q.exec();
  }
}

void Database::setupCarsModel() {
  carsModel_ = std::make_unique<QSqlTableModel>(nullptr, db_);
  carsModel_->setTable("cars");
  carsModel_->setEditStrategy(QSqlTableModel::OnManualSubmit);
  carsModel_->select();
  carsModel_->setHeaderData(0, Qt::Horizontal, QObject::tr("ID"));
  carsModel_->setHeaderData(1, Qt::Horizontal, QObject::tr("Brand"));
  carsModel_->setHeaderData(2, Qt::Horizontal, QObject::tr("Model"));
  carsModel_->setHeaderData(3, Qt::Horizontal, QObject::tr("Year"));
  carsModel_->setHeaderData(4, Qt::Horizontal, QObject::tr("Price / day"));
}

QSqlTableModel *Database::carsModel() { return carsModel_.get(); }

bool Database::insertCar(const QString &brand, const QString &model, int year,
                         double pricePerDay, int quantity,
                         const QString &description, const QString &imagePath) {
  QSqlQuery q(db_);
  q.prepare("INSERT INTO cars (brand, model, year, price_per_day, quantity, "
            "description, image_path) "
            "VALUES (?, ?, ?, ?, ?, ?, ?)");
  q.addBindValue(brand);
  q.addBindValue(model);
  q.addBindValue(year);
  q.addBindValue(pricePerDay);
  q.addBindValue(qBound(1, quantity, 10)); // Ограничиваем от 1 до 10
  q.addBindValue(description);
  q.addBindValue(imagePath);
  const bool ok = q.exec();
  if (ok && carsModel_)
    carsModel_->select();
  return ok;
}

bool Database::updateCar(int id, const QString &brand, const QString &model,
                         int year, double pricePerDay, int quantity,
                         const QString &description, const QString &imagePath) {
  QSqlQuery q(db_);
  q.prepare(
      "UPDATE cars SET brand=?, model=?, year=?, price_per_day=?, quantity=?, "
      "description=?, image_path=? WHERE id=?");
  q.addBindValue(brand);
  q.addBindValue(model);
  q.addBindValue(year);
  q.addBindValue(pricePerDay);
  q.addBindValue(qBound(1, quantity, 10)); // Ограничиваем от 1 до 10
  q.addBindValue(description);
  q.addBindValue(imagePath);
  q.addBindValue(id);
  const bool ok = q.exec();
  if (ok && carsModel_)
    carsModel_->select();
  return ok;
}

bool Database::deleteCar(int id) {
  QSqlQuery q(db_);
  q.prepare("DELETE FROM cars WHERE id=?");
  q.addBindValue(id);
  const bool ok = q.exec();
  if (ok && carsModel_)
    carsModel_->select();
  return ok;
}

// Cars for users
QSqlQueryModel *Database::getAvailableCars(int customerId) {
  QSqlQueryModel *model = new QSqlQueryModel();
  QString query;
  if (customerId > 0) {
    query =
        QString(
            "SELECT c.id, c.brand, c.model, c.year, c.price_per_day, "
            "c.description, "
            "c.available, c.image_path, "
            "CASE WHEN b.id IS NOT NULL THEN 1 ELSE 0 END AS bookmarked, "
            "c.engine_type, c.engine_capacity, c.power_hp, c.seats, c.quantity "
            "FROM cars c "
            "LEFT JOIN bookmarks b ON c.id = b.car_id AND b.customer_id = %1 "
            "WHERE c.available = 1 ORDER BY c.brand, c.model")
            .arg(customerId);
  } else {
    query = "SELECT id, brand, model, year, price_per_day, description, "
            "available, image_path, 0 AS bookmarked, engine_type, "
            "engine_capacity, power_hp, seats, quantity "
            "FROM cars WHERE available = 1 ORDER BY brand, model";
  }
  model->setQuery(query, db_);
  model->setHeaderData(1, Qt::Horizontal, "Марка");
  model->setHeaderData(2, Qt::Horizontal, "Модель");
  model->setHeaderData(3, Qt::Horizontal, "Год");
  model->setHeaderData(4, Qt::Horizontal, "Цена/день (руб)");
  model->setHeaderData(5, Qt::Horizontal, "Описание");
  return model;
}

CarInfo Database::getCar(int id, int customerId) {
  CarInfo info;
  QSqlQuery q(db_);
  if (customerId > 0) {
    q.prepare(
        "SELECT c.id, c.brand, c.model, c.year, c.price_per_day, c.quantity, "
        "c.description, c.available, c.image_path, "
        "CASE WHEN b.id IS NOT NULL THEN 1 ELSE 0 END, "
        "c.engine_type, c.engine_capacity, c.power_hp, c.seats "
        "FROM cars c "
        "LEFT JOIN bookmarks b ON c.id = b.car_id AND b.customer_id = ? "
        "WHERE c.id = ?");
    q.addBindValue(customerId);
    q.addBindValue(id);
  } else {
    q.prepare(
        "SELECT id, brand, model, year, price_per_day, quantity, description, "
        "available, image_path, 0, engine_type, engine_capacity, "
        "power_hp, seats FROM cars WHERE id = ?");
    q.addBindValue(id);
  }
  if (q.exec() && q.next()) {
    info.id = q.value(0).toInt();
    info.brand = q.value(1).toString();
    info.model = q.value(2).toString();
    info.year = q.value(3).toInt();
    info.pricePerDay = q.value(4).toDouble();
    info.quantity = q.value(5).toInt();
    info.description = q.value(6).toString();
    info.available = q.value(7).toBool();
    info.imagePath = q.value(8).toString();
    info.bookmarked = q.value(9).toInt() != 0;
    info.engineType = q.value(10).toString();
    info.engineCapacityL = q.value(11).toDouble();
    info.powerHp = q.value(12).toInt();
    info.seats = q.value(13).toInt();
  }
  return info;
}

QSqlQueryModel *Database::getBookmarkedCars(int customerId) {
  QSqlQueryModel *model = new QSqlQueryModel();
  QSqlQuery q(db_);
  q.prepare(
      "SELECT c.id, c.brand, c.model, c.year, c.price_per_day, c.description, "
      "c.available, c.image_path, c.engine_type, c.engine_capacity, "
      "c.power_hp, c.seats, c.quantity "
      "FROM cars c "
      "INNER JOIN bookmarks b ON c.id = b.car_id "
      "WHERE b.customer_id = ? ORDER BY c.brand, c.model");
  q.addBindValue(customerId);
  q.exec();
  model->setQuery(std::move(q));
  return model;
}

bool Database::setBookmarked(int carId, int customerId, bool bookmarked) {
  QSqlQuery q(db_);
  if (bookmarked) {
    // Добавляем закладку, игнорируя конфликты (если уже есть)
    q.prepare(
        "INSERT OR IGNORE INTO bookmarks (customer_id, car_id) VALUES (?, ?)");
    q.addBindValue(customerId);
    q.addBindValue(carId);
  } else {
    // Удаляем закладку
    q.prepare("DELETE FROM bookmarks WHERE customer_id = ? AND car_id = ?");
    q.addBindValue(customerId);
    q.addBindValue(carId);
  }
  bool ok = q.exec();
  if (ok && carsModel_)
    carsModel_->select();
  return ok;
}

bool Database::isCarAvailable(int carId, const QDate &startDate,
                              const QDate &endDate) {
  return getAvailableQuantity(carId, startDate, endDate) > 0;
}

int Database::getAvailableQuantity(int carId, const QDate &startDate,
                                   const QDate &endDate) {
  // Получаем общее количество экземпляров
  QSqlQuery qTotal(db_);
  qTotal.prepare("SELECT quantity FROM cars WHERE id = ?");
  qTotal.addBindValue(carId);
  int totalQuantity = 1;
  if (qTotal.exec() && qTotal.next()) {
    totalQuantity = qTotal.value(0).toInt();
  }

  // Считаем количество забронированных экземпляров на перекрывающиеся периоды
  QSqlQuery q(db_);
  q.prepare(
      "SELECT COUNT(*) FROM rentals WHERE car_id = ? AND status = 'active' "
      "AND ((start_date <= ? AND end_date >= ?) OR (start_date <= ? AND "
      "end_date >= ?) OR "
      "(start_date >= ? AND end_date <= ?))");
  q.addBindValue(carId);
  q.addBindValue(endDate.toString(Qt::ISODate));
  q.addBindValue(endDate.toString(Qt::ISODate));
  q.addBindValue(startDate.toString(Qt::ISODate));
  q.addBindValue(startDate.toString(Qt::ISODate));
  q.addBindValue(startDate.toString(Qt::ISODate));
  q.addBindValue(endDate.toString(Qt::ISODate));

  int bookedCount = 0;
  if (q.exec() && q.next()) {
    bookedCount = q.value(0).toInt();
  }

  // Доступное количество = общее - забронированное
  return qMax(0, totalQuantity - bookedCount);
}

// Customers
int Database::addCustomer(const QString &name, const QString &phone,
                          const QString &passwordHash) {
  QSqlQuery q(db_);
  q.prepare("INSERT INTO customers (name, phone, email, password_hash) VALUES "
            "(?, ?, '', ?)");
  q.addBindValue(name);
  q.addBindValue(phone);
  q.addBindValue(passwordHash);
  if (q.exec()) {
    return q.lastInsertId().toInt();
  }
  return -1;
}

CustomerInfo Database::getCustomer(int id) {
  CustomerInfo info;
  QSqlQuery q(db_);
  q.prepare("SELECT id, name, phone, email FROM customers WHERE id = ?");
  q.addBindValue(id);
  if (q.exec() && q.next()) {
    info.id = q.value(0).toInt();
    info.name = q.value(1).toString();
    info.phone = q.value(2).toString();
    info.email = q.value(3).toString();
  }
  return info;
}

QSqlQueryModel *Database::getCustomersModel() {
  QSqlQueryModel *model = new QSqlQueryModel();
  model->setQuery("SELECT id, name, phone, email FROM customers ORDER BY name",
                  db_);
  model->setHeaderData(1, Qt::Horizontal, "Имя");
  model->setHeaderData(2, Qt::Horizontal, "Телефон");
  model->setHeaderData(3, Qt::Horizontal, "Email");
  return model;
}

int Database::findCustomerByPhoneAndPassword(const QString &phone,
                                             const QString &passwordHash) {
  QSqlQuery q(db_);
  q.prepare("SELECT id FROM customers WHERE phone=? AND password_hash=?");
  q.addBindValue(phone);
  q.addBindValue(passwordHash);
  if (q.exec() && q.next())
    return q.value(0).toInt();
  return -1;
}

// Rentals
int Database::createRental(int carId, int customerId, const QDate &startDate,
                           const QDate &endDate, double totalPrice) {
  QSqlQuery q(db_);
  q.prepare("INSERT INTO rentals (car_id, customer_id, start_date, end_date, "
            "total_price, status) "
            "VALUES (?, ?, ?, ?, ?, 'active')");
  q.addBindValue(carId);
  q.addBindValue(customerId);
  q.addBindValue(startDate.toString(Qt::ISODate));
  q.addBindValue(endDate.toString(Qt::ISODate));
  q.addBindValue(totalPrice);
  if (q.exec()) {
    return q.lastInsertId().toInt();
  }
  return -1;
}

QSqlQueryModel *Database::getCustomerRentals(int customerId) {
  QSqlQueryModel *model = new QSqlQueryModel();
  QString query = QString("SELECT r.id, c.brand || ' ' || c.model as car_name, "
                          "r.start_date, r.end_date, r.total_price, r.status "
                          "FROM rentals r JOIN cars c ON r.car_id = c.id "
                          "WHERE r.customer_id = %1 ORDER BY r.start_date DESC")
                      .arg(customerId);
  model->setQuery(query, db_);
  model->setHeaderData(1, Qt::Horizontal, "Автомобиль");
  model->setHeaderData(2, Qt::Horizontal, "Дата начала");
  model->setHeaderData(3, Qt::Horizontal, "Дата окончания");
  model->setHeaderData(4, Qt::Horizontal, "Сумма (руб)");
  model->setHeaderData(5, Qt::Horizontal, "Статус");
  return model;
}

QSqlQueryModel *Database::getAllRentals() {
  QSqlQueryModel *model = new QSqlQueryModel();
  model->setQuery(
      "SELECT r.id, c.brand || ' ' || c.model as car_name, "
      "cust.name, r.start_date, r.end_date, r.total_price, r.status "
      "FROM rentals r JOIN cars c ON r.car_id = c.id "
      "JOIN customers cust ON r.customer_id = cust.id "
      "ORDER BY r.start_date DESC",
      db_);
  model->setHeaderData(1, Qt::Horizontal, "Автомобиль");
  model->setHeaderData(2, Qt::Horizontal, "Клиент");
  model->setHeaderData(3, Qt::Horizontal, "Дата начала");
  model->setHeaderData(4, Qt::Horizontal, "Дата окончания");
  model->setHeaderData(5, Qt::Horizontal, "Сумма (руб)");
  model->setHeaderData(6, Qt::Horizontal, "Статус");
  return model;
}

bool Database::cancelRental(int rentalId) {
  QSqlQuery q(db_);
  q.prepare("UPDATE rentals SET status = 'cancelled' WHERE id = ?");
  q.addBindValue(rentalId);
  return q.exec();
}

bool Database::completeRental(int rentalId) {
  QSqlQuery q(db_);
  q.prepare("UPDATE rentals SET status = 'completed' WHERE id = ?");
  q.addBindValue(rentalId);
  return q.exec();
}
