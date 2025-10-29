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
              "price_per_hour REAL NOT NULL,"
              "description TEXT,"
              "available INTEGER DEFAULT 1,"
              "image_path TEXT"
              ")")) {
    return false;
  }

  // Backward compatibility: if old column price_per_day exists, rename/migrate
  if (q.exec("PRAGMA table_info(cars)")) {
    bool hasPricePerDay = false;
    bool hasPricePerHour = false;
    while (q.next()) {
      const QString name = q.value(1).toString();
      if (name == "price_per_day")
        hasPricePerDay = true;
      if (name == "price_per_hour")
        hasPricePerHour = true;
    }
    if (hasPricePerDay && !hasPricePerHour) {
      QSqlQuery alter(db_);
      alter.exec("ALTER TABLE cars ADD COLUMN price_per_hour REAL");
      alter.exec("UPDATE cars SET price_per_hour = price_per_day / 24.0 WHERE "
                 "price_per_hour IS NULL");
    }
    // add image_path if missing
    QSqlQuery info2(db_);
    info2.exec("PRAGMA table_info(cars)");
    bool hasImagePath = false;
    while (info2.next()) {
      if (info2.value(1).toString() == "image_path") {
        hasImagePath = true;
        break;
      }
    }
    if (!hasImagePath) {
      QSqlQuery addImg(db_);
      addImg.exec("ALTER TABLE cars ADD COLUMN image_path TEXT");
    }
  }

  // Customers table
  if (!q.exec("CREATE TABLE IF NOT EXISTS customers ("
              "id INTEGER PRIMARY KEY AUTOINCREMENT,"
              "name TEXT NOT NULL,"
              "phone TEXT NOT NULL,"
              "email TEXT"
              ")")) {
    return false;
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
    // Premium класс
    insertCar("BMW", "3 Series", 2024, 5.5, "Спортивный седан премиум класса",
              "");
    insertCar("BMW", "5 Series", 2024, 7.0, "Роскошный бизнес-седан", "");
    insertCar("BMW", "X5", 2023, 8.5, "Премиум внедорожник", "");
    insertCar("Mercedes-Benz", "C-Class", 2023, 6.0, "Элегантность и комфорт",
              "");
    insertCar("Mercedes-Benz", "E-Class", 2024, 7.5,
              "Бизнес-класс высшего уровня", "");
    insertCar("Mercedes-Benz", "GLE", 2023, 9.0, "Премиум кроссовер", "");
    insertCar("Audi", "A4", 2024, 5.0, "Динамика и стиль", "");
    insertCar("Audi", "A6", 2024, 6.5, "Премиум седан", "");
    insertCar("Audi", "Q5", 2023, 7.2, "Спортивный кроссовер", "");
    insertCar("Lexus", "ES", 2024, 6.2, "Японская роскошь", "");
    insertCar("Lexus", "RX", 2023, 7.8, "Премиум внедорожник", "");

    // Средний класс
    insertCar("Toyota", "Camry", 2023, 3.0,
              "Комфортный седан для длительных поездок", "");
    insertCar("Toyota", "RAV4", 2024, 4.0, "Надежный кроссовер", "");
    insertCar("Toyota", "Corolla", 2023, 2.5, "Экономичный и практичный", "");
    insertCar("Honda", "Accord", 2023, 3.2, "Проверенная надежность", "");
    insertCar("Honda", "CR-V", 2024, 3.8, "Компактный внедорожник", "");
    insertCar("Honda", "Civic", 2023, 2.8, "Спортивный седан", "");
    insertCar("Volkswagen", "Passat", 2023, 3.5, "Надежный семейный автомобиль",
              "");
    insertCar("Volkswagen", "Tiguan", 2024, 4.2, "Семейный кроссовер", "");
    insertCar("Volkswagen", "Golf", 2023, 3.0, "Компактный хетчбек", "");
    insertCar("Ford", "Focus", 2023, 2.8, "Компактный и экономичный", "");
    insertCar("Ford", "Escape", 2024, 3.6, "Городской кроссовер", "");
    insertCar("Ford", "Mustang", 2023, 6.5, "Легендарный спорткар", "");

    // Бюджетный класс
    insertCar("Hyundai", "Elantra", 2024, 2.9,
              "Современный дизайн, отличная цена", "");
    insertCar("Hyundai", "Tucson", 2023, 3.4, "Практичный кроссовер", "");
    insertCar("Hyundai", "Sonata", 2024, 3.1, "Стильный седан", "");
    insertCar("Kia", "Optima", 2023, 3.0, "Комфортный седан", "");
    insertCar("Kia", "Sportage", 2024, 3.5, "Универсальный кроссовер", "");
    insertCar("Kia", "Rio", 2023, 2.4, "Экономичный городской автомобиль", "");
    insertCar("Nissan", "Altima", 2024, 3.2, "Надежный седан", "");
    insertCar("Nissan", "Rogue", 2023, 3.7, "Популярный кроссовер", "");
    insertCar("Nissan", "Sentra", 2024, 2.6, "Компактный седан", "");
    insertCar("Mazda", "CX-5", 2023, 3.9, "Спортивный кроссовер", "");
    insertCar("Mazda", "Mazda6", 2024, 3.4, "Элегантный седан", "");
    insertCar("Mazda", "CX-3", 2023, 3.2, "Компактный кроссовер", "");

    // Люкс класс
    insertCar("Porsche", "911", 2024, 15.0, "Икона спортивных автомобилей", "");
    insertCar("Porsche", "Cayenne", 2023, 12.0, "Роскошный внедорожник", "");
    insertCar("Tesla", "Model 3", 2024, 8.0, "Электрический седан", "");
    insertCar("Tesla", "Model Y", 2024, 9.5, "Электрический кроссовер", "");
    insertCar("Tesla", "Model S", 2024, 14.0, "Премиум электромобиль", "");
    insertCar("Jaguar", "XF", 2023, 7.8, "Британская роскошь", "");
    insertCar("Jaguar", "F-Pace", 2024, 8.5, "Премиум внедорожник", "");
    insertCar("Range Rover", "Evoque", 2023, 10.0,
              "Компактный премиум внедорожник", "");
    insertCar("Range Rover", "Sport", 2024, 13.0,
              "Спортивный премиум внедорожник", "");
    insertCar("Volvo", "XC60", 2023, 5.8, "Безопасный кроссовер", "");
    insertCar("Volvo", "S90", 2024, 6.2, "Премиум седан", "");
    insertCar("Genesis", "G80", 2024, 6.8, "Роскошный седан", "");
    insertCar("Genesis", "GV70", 2023, 7.2, "Премиум кроссовер", "");
    insertCar("Infiniti", "Q50", 2024, 5.5, "Спортивный премиум седан", "");
    insertCar("Infiniti", "QX50", 2023, 6.0, "Премиум кроссовер", "");

    // Спортивные
    insertCar("Chevrolet", "Camaro", 2024, 7.5, "Американский мускул-кар", "");
    insertCar("Chevrolet", "Corvette", 2024, 12.5, "Легендарный спорткар", "");
    insertCar("Dodge", "Challenger", 2023, 7.0, "Мощный спорткар", "");
    insertCar("Subaru", "WRX", 2024, 5.0, "Спортивный седан с полным приводом",
              "");
    insertCar("Subaru", "Outback", 2023, 4.2, "Универсал для путешествий", "");
    insertCar("Mitsubishi", "Outlander", 2024, 3.6, "Семейный внедорожник", "");
  } else if (count < 50) {
    // Если автомобилей меньше 50, дополняем до 50
    // Список всех 50 автомобилей (уже вставлены выше)
    // Эта логика может быть расширена для добавления недостающих
    qDebug() << "В базе данных" << count << "автомобилей, необходимо 50";
  }

  return true;
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
  carsModel_->setHeaderData(4, Qt::Horizontal, QObject::tr("Price / hour"));
}

QSqlTableModel *Database::carsModel() { return carsModel_.get(); }

bool Database::insertCar(const QString &brand, const QString &model, int year,
                         double pricePerHour, const QString &description,
                         const QString &imagePath) {
  QSqlQuery q(db_);
  q.prepare("INSERT INTO cars (brand, model, year, price_per_hour, "
            "description, image_path) "
            "VALUES (?, ?, ?, ?, ?, ?)");
  q.addBindValue(brand);
  q.addBindValue(model);
  q.addBindValue(year);
  q.addBindValue(pricePerHour);
  q.addBindValue(description);
  q.addBindValue(imagePath);
  const bool ok = q.exec();
  if (ok && carsModel_)
    carsModel_->select();
  return ok;
}

bool Database::updateCar(int id, const QString &brand, const QString &model,
                         int year, double pricePerHour,
                         const QString &description, const QString &imagePath) {
  QSqlQuery q(db_);
  q.prepare("UPDATE cars SET brand=?, model=?, year=?, price_per_hour=?, "
            "description=?, image_path=? WHERE id=?");
  q.addBindValue(brand);
  q.addBindValue(model);
  q.addBindValue(year);
  q.addBindValue(pricePerHour);
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
QSqlQueryModel *Database::getAvailableCars() {
  QSqlQueryModel *model = new QSqlQueryModel();
  model->setQuery("SELECT id, brand, model, year, price_per_hour, description, "
                  "available, image_path "
                  "FROM cars WHERE available = 1 ORDER BY brand, model",
                  db_);
  model->setHeaderData(1, Qt::Horizontal, "Марка");
  model->setHeaderData(2, Qt::Horizontal, "Модель");
  model->setHeaderData(3, Qt::Horizontal, "Год");
  model->setHeaderData(4, Qt::Horizontal, "Цена/день (руб)");
  model->setHeaderData(5, Qt::Horizontal, "Описание");
  return model;
}

CarInfo Database::getCar(int id) {
  CarInfo info;
  QSqlQuery q(db_);
  q.prepare("SELECT id, brand, model, year, price_per_hour, description, "
            "available, image_path FROM cars WHERE id = ?");
  q.addBindValue(id);
  if (q.exec() && q.next()) {
    info.id = q.value(0).toInt();
    info.brand = q.value(1).toString();
    info.model = q.value(2).toString();
    info.year = q.value(3).toInt();
    info.pricePerHour = q.value(4).toDouble();
    info.description = q.value(5).toString();
    info.available = q.value(6).toBool();
    info.imagePath = q.value(7).toString();
  }
  return info;
}

bool Database::isCarAvailable(int carId, const QDate &startDate,
                              const QDate &endDate) {
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

  if (q.exec() && q.next()) {
    return q.value(0).toInt() == 0;
  }
  return false;
}

// Customers
int Database::addCustomer(const QString &name, const QString &phone,
                          const QString &email) {
  QSqlQuery q(db_);
  q.prepare("INSERT INTO customers (name, phone, email) VALUES (?, ?, ?)");
  q.addBindValue(name);
  q.addBindValue(phone);
  q.addBindValue(email);
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
