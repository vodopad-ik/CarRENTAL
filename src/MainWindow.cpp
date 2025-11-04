#include "MainWindow.h"
#include "CarCardWidget.h"
#include "LoginDialog.h"
#include "RentalDialog.h"
#include "db/Database.h"

#include <QApplication>
#include <QComboBox>
#include <QDialog>
#include <QDir>
#include <QDoubleSpinBox>
#include <QFile>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QLineEdit>
#include <QList>
#include <QMessageBox>
#include <QPushButton>
#include <QScrollArea>
#include <QSpinBox>
#include <QTabWidget>
#include <QTableView>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), currentCustomerId_(-1), currentCurrency_("USD") {
  setWindowTitle("Аренда автомобилей");
  resize(1200, 800);

  // Initialize database
  if (!Database::instance().initialize()) {
    QMessageBox::critical(this, "Ошибка",
                          "Не удалось инициализировать базу данных");
  }

  setupUI();
  // попытка автологина
  QFile sessionFile(QDir::homePath() + "/.carrental/session.json");
  if (sessionFile.exists() && sessionFile.open(QIODevice::ReadOnly)) {
    auto doc = QJsonDocument::fromJson(sessionFile.readAll());
    sessionFile.close();
    int sid = doc.object().value("customerId").toInt(-1);
    QString sname = doc.object().value("name").toString();
    if (sid > 0) {
      currentCustomerId_ = sid;
      currentCustomerName_ = sname;
      welcomeLabel_->setText(
          QString("Добро пожаловать, %1!").arg(currentCustomerName_));
      tabs_->setVisible(true);
      myRentalsBtn_->setVisible(true);
      logoutBtn_->setVisible(true);
      loadCars();
      // Предзагружаем аренды, чтобы они были готовы при переключении на вкладку
      // Загружаем данные без переключения вкладки
      if (rentalsModel_) {
        delete rentalsModel_;
        rentalsModel_ = nullptr;
      }
      rentalsModel_ =
          Database::instance().getCustomerRentals(currentCustomerId_);
      rentalsTable_->setModel(rentalsModel_);
      rentalsTable_->resizeColumnsToContents();
      rentalsTable_->horizontalHeader()->setStretchLastSection(true);
      // Убеждаемся, что активна вкладка "Автомобили" (первая вкладка)
      tabs_->setCurrentIndex(0);
      this->show(); // Показываем окно при успешном автологине
    } else {
      showLogin();
    }
  } else {
    showLogin();
  }
}

MainWindow::~MainWindow() {
  if (rentalsModel_) {
    delete rentalsModel_;
  }
}

void MainWindow::setupUI() {
  centralWidget_ = new QWidget(this);
  setCentralWidget(centralWidget_);

  auto *mainLayout = new QVBoxLayout(centralWidget_);
  mainLayout->setSpacing(0);
  mainLayout->setContentsMargins(0, 0, 0, 0);

  // Header
  auto *header = new QWidget(this);
  header->setStyleSheet("background-color: #2196F3; padding: 15px;");
  auto *headerLayout = new QHBoxLayout(header);

  welcomeLabel_ = new QLabel("Добро пожаловать", header);
  welcomeLabel_->setStyleSheet(
      "color: white; font-size: 18px; font-weight: bold;");
  headerLayout->addWidget(welcomeLabel_);

  headerLayout->addStretch();

  myRentalsBtn_ = new QPushButton("Мои аренды", header);
  // Единый стиль кнопок в шапке
  const QString headerBtnStyle =
      "QPushButton { background-color: white; color: #2196F3; padding: 8px "
      "16px; border-radius: 6px; }"
      "QPushButton:hover { filter: brightness(0.95); }"
      "QPushButton:pressed { transform: translateY(1px); }";
  myRentalsBtn_->setStyleSheet(headerBtnStyle);
  myRentalsBtn_->setIcon(style()->standardIcon(QStyle::SP_FileDialogListView));
  myRentalsBtn_->setMinimumHeight(36);
  headerLayout->addWidget(myRentalsBtn_);

  // Currency selector
  currencyBox_ = new QComboBox(header);
  currencyBox_->addItem("USD");
  currencyBox_->addItem("EUR");
  currencyBox_->addItem("BYN");
  currencyBox_->setMinimumHeight(36);
  currencyBox_->setStyleSheet(
      "QComboBox { background-color: white; color: #2196F3; padding: 6px 12px; "
      "border-radius: 6px; border: 1px solid #e0e0e0; }"
      "QComboBox:hover { border-color: #bdbdbd; }"
      "QComboBox::drop-down { width: 26px; border: none; }"
      "QComboBox QAbstractItemView { background: white; "
      "selection-background-color: #E3F2FD; }");
  headerLayout->addWidget(currencyBox_);

  logoutBtn_ = new QPushButton("Выйти", header);
  logoutBtn_->setStyleSheet(headerBtnStyle);
  logoutBtn_->setIcon(style()->standardIcon(QStyle::SP_DialogCloseButton));
  logoutBtn_->setMinimumHeight(36);
  headerLayout->addWidget(logoutBtn_);

  mainLayout->addWidget(header);

  // Tabs
  tabs_ = new QTabWidget(this);
  tabs_->setStyleSheet("QTabWidget::pane { border: none; }");

  // Cars tab
  auto *carsTab = new QWidget();
  auto *carsLayout = new QVBoxLayout(carsTab);

  // Search bar
  auto *searchLayout = new QHBoxLayout();
  searchEdit_ = new QLineEdit();
  searchEdit_->setPlaceholderText("Поиск по марке, модели...");
  searchEdit_->setStyleSheet(
      "QLineEdit { padding: 10px; font-size: 14px; border: 2px solid #e0e0e0; "
      "border-radius: 5px; }"
      "QLineEdit:focus { border-color: #2196F3; }");
  searchLayout->addWidget(searchEdit_);

  // Фильтры
  engineTypeFilter_ = new QComboBox();
  engineTypeFilter_->addItem("Любой");
  engineTypeFilter_->addItem("EV");
  engineTypeFilter_->addItem("Hybrid");
  engineTypeFilter_->addItem("ICE");
  engineTypeFilter_->setMinimumHeight(38);
  searchLayout->addWidget(engineTypeFilter_);

  seatsMinFilter_ = new QSpinBox();
  seatsMinFilter_->setRange(0, 9);
  seatsMinFilter_->setPrefix("Мест ≥ ");
  seatsMinFilter_->setMinimumHeight(38);
  searchLayout->addWidget(seatsMinFilter_);

  powerMinFilter_ = new QSpinBox();
  powerMinFilter_->setRange(0, 2000);
  powerMinFilter_->setPrefix("ЛС ≥ ");
  powerMinFilter_->setMinimumHeight(38);
  searchLayout->addWidget(powerMinFilter_);

  capacityMinFilter_ = new QDoubleSpinBox();
  capacityMinFilter_->setRange(0.0, 10.0);
  capacityMinFilter_->setDecimals(1);
  capacityMinFilter_->setSingleStep(0.1);
  capacityMinFilter_->setPrefix("Литры ≥ ");
  capacityMinFilter_->setMinimumHeight(38);
  searchLayout->addWidget(capacityMinFilter_);

  carsLayout->addLayout(searchLayout);

  // Scroll area for cars
  scrollArea_ = new QScrollArea();
  scrollArea_->setWidgetResizable(true);
  scrollArea_->setStyleSheet(
      "QScrollArea { border: none; background-color: #f5f5f5; }");

  carsContainer_ = new QWidget();
  carsContainer_->setStyleSheet("background-color: #f5f5f5;");
  auto *gridLayout = new QGridLayout(carsContainer_);
  gridLayout->setSpacing(20);
  gridLayout->setContentsMargins(20, 20, 20, 20);
  // Отключаем растягивание колонок, чтобы карточки не растягивались при малом
  // количестве
  for (int i = 0; i < 3; ++i) {
    gridLayout->setColumnStretch(i, 0);
    gridLayout->setColumnMinimumWidth(
        i, 320); // Минимальная ширина колонки = минимальная ширина карточки
  }

  scrollArea_->setWidget(carsContainer_);
  carsLayout->addWidget(scrollArea_);

  tabs_->addTab(carsTab, "Автомобили");

  // Bookmarks tab
  bookmarksContainer_ = new QWidget();
  auto *bmLayoutOuter = new QVBoxLayout(bookmarksContainer_);
  auto *bmScroll = new QScrollArea();
  bmScroll->setWidgetResizable(true);
  bmScroll->setStyleSheet(
      "QScrollArea { border: none; background-color: white; }");
  bmInner_ = new QWidget();
  bmInner_->setStyleSheet("background-color: white;");
  auto *bmGrid = new QGridLayout(bmInner_);
  bmGrid->setSpacing(20);
  bmGrid->setContentsMargins(20, 20, 20, 20);
  // Отключаем растягивание колонок, чтобы карточки не растягивались при малом
  // количестве
  for (int i = 0; i < 3; ++i) {
    bmGrid->setColumnStretch(i, 0);
    bmGrid->setColumnMinimumWidth(
        i, 320); // Минимальная ширина колонки = минимальная ширина карточки
  }
  bmScroll->setWidget(bmInner_);
  bmLayoutOuter->addWidget(bmScroll);
  tabs_->addTab(bookmarksContainer_, "Закладки");

  // My Rentals tab
  rentalsWidget_ = new QWidget();
  auto *rentalsLayout = new QVBoxLayout(rentalsWidget_);
  rentalsTable_ = new QTableView();
  rentalsTable_->setStyleSheet("QTableView { border: none; }");
  rentalsLayout->addWidget(rentalsTable_);
  tabs_->addTab(rentalsWidget_, "Мои аренды");

  mainLayout->addWidget(tabs_);

  // Connections
  connect(currencyBox_, &QComboBox::currentTextChanged, this,
          &MainWindow::onCurrencyChanged);
  connect(myRentalsBtn_, &QPushButton::clicked, this,
          &MainWindow::onShowMyRentals);
  connect(tabs_, &QTabWidget::currentChanged, this, [this](int idx) {
    if (tabs_->tabText(idx) == "Закладки")
      loadBookmarks();
    else if (tabs_->tabText(idx) == "Мои аренды")
      onShowMyRentals();
  });
  connect(logoutBtn_, &QPushButton::clicked, this, &MainWindow::onLogout);
  connect(searchEdit_, &QLineEdit::textChanged, this,
          &MainWindow::onSearchChanged);
  connect(engineTypeFilter_, &QComboBox::currentTextChanged, this,
          [this](const QString &) { loadCars(); });
  connect(seatsMinFilter_, qOverload<int>(&QSpinBox::valueChanged), this,
          [this](int) { loadCars(); });
  connect(powerMinFilter_, qOverload<int>(&QSpinBox::valueChanged), this,
          [this](int) { loadCars(); });
  connect(capacityMinFilter_, qOverload<double>(&QDoubleSpinBox::valueChanged),
          this, [this](double) { loadCars(); });

  // Initially hide tabs until login
  tabs_->setVisible(false);
  myRentalsBtn_->setVisible(false);
  logoutBtn_->setVisible(false);
}

void MainWindow::showLogin() {
  this->hide(); // Скрываем главное окно перед показом диалога входа
  LoginDialog dialog(this);
  if (dialog.exec() == QDialog::Accepted) {
    currentCustomerId_ = dialog.customerId();
    currentCustomerName_ = dialog.customerName();
    welcomeLabel_->setText(
        QString("Добро пожаловать, %1!").arg(currentCustomerName_));
    // сохранить сессию
    QJsonObject obj;
    obj["customerId"] = currentCustomerId_;
    obj["name"] = currentCustomerName_;
    QJsonDocument doc(obj);
    QFile session(QDir::homePath() + "/.carrental/session.json");
    QDir().mkpath(QDir::homePath() + "/.carrental");
    if (session.open(QIODevice::WriteOnly)) {
      session.write(doc.toJson());
      session.close();
    }
    tabs_->setVisible(true);
    myRentalsBtn_->setVisible(true);
    logoutBtn_->setVisible(true);
    loadCars();
    // Предзагружаем аренды, чтобы они были готовы при переключении на вкладку
    // Загружаем данные без переключения вкладки
    if (rentalsModel_) {
      delete rentalsModel_;
      rentalsModel_ = nullptr;
    }
    rentalsModel_ = Database::instance().getCustomerRentals(currentCustomerId_);
    rentalsTable_->setModel(rentalsModel_);
    rentalsTable_->resizeColumnsToContents();
    rentalsTable_->horizontalHeader()->setStretchLastSection(true);
    // Убеждаемся, что активна вкладка "Автомобили" (первая вкладка)
    tabs_->setCurrentIndex(0);
    this->show();
  } else {
    // Если диалог закрыт без входа, закрываем приложение
    QApplication::quit();
  }
}

void MainWindow::loadCars() {
  // Clear existing cards
  for (auto *card : carCards_) {
    card->deleteLater();
  }
  carCards_.clear();

  auto *model = Database::instance().getAvailableCars(currentCustomerId_);
  auto *gridLayout = qobject_cast<QGridLayout *>(carsContainer_->layout());

  QString searchText = searchEdit_->text().toLower();
  int row = 0, col = 0;
  const int colsPerRow = 3;

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
    if (model->columnCount() >= 14) {
      car.bookmarked = model->data(model->index(i, 8)).toInt() != 0;
      car.engineType = model->data(model->index(i, 9)).toString();
      car.engineCapacityL = model->data(model->index(i, 10)).toDouble();
      car.powerHp = model->data(model->index(i, 11)).toInt();
      car.seats = model->data(model->index(i, 12)).toInt();
      car.quantity = model->data(model->index(i, 13)).toInt();
    } else {
      car.bookmarked = model->data(model->index(i, 8)).toInt() != 0;
      car.quantity = 1; // По умолчанию 1 экземпляр
    }
    QString carText =
        (car.brand + " " + car.model + " " + QString::number(car.year))
            .toLower();
    if (!searchText.isEmpty() && !carText.contains(searchText)) {
      continue;
    }

    // Фильтрация по дополнительным полям
    const QString typeSel = engineTypeFilter_->currentText();
    if (typeSel != "Любой" && !car.engineType.isEmpty() &&
        car.engineType != typeSel)
      continue;
    if (seatsMinFilter_->value() > 0 && car.seats < seatsMinFilter_->value())
      continue;
    if (powerMinFilter_->value() > 0 && car.powerHp < powerMinFilter_->value())
      continue;
    if (capacityMinFilter_->value() > 0.0 &&
        car.engineCapacityL < capacityMinFilter_->value())
      continue;

    auto *card = new CarCardWidget(car, currentCurrency_, carsContainer_);
    connect(card, &CarCardWidget::rentClicked, this,
            &MainWindow::onCarRentClicked);
    connect(card, &CarCardWidget::bookmarkToggled, this,
            &MainWindow::onBookmarkToggled);
    gridLayout->addWidget(card, row, col, 1, 1, Qt::AlignLeft | Qt::AlignTop);
    carCards_.append(card);

    col++;
    if (col >= colsPerRow) {
      col = 0;
      row++;
    }
  }

  delete model;
}

void MainWindow::loadBookmarks() {
  // clear children of bmInner_
  auto *grid = qobject_cast<QGridLayout *>(bmInner_->layout());
  if (!grid)
    grid = new QGridLayout(bmInner_);
  QLayoutItem *child;
  while ((child = grid->takeAt(0)) != nullptr) {
    if (auto w = child->widget())
      w->deleteLater();
    delete child;
  }
  auto *model = Database::instance().getBookmarkedCars(currentCustomerId_);
  int row = 0, col = 0;
  const int colsPerRow = 3;
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
    car.bookmarked = true; // Всегда true в закладках
    // getBookmarkedCars возвращает 13 колонок: id, brand, model, year,
    // price_per_day, description, available, image_path, engine_type,
    // engine_capacity, power_hp, seats, quantity (без поля bookmarked, так как
    // все здесь уже в закладках)
    if (model->columnCount() >= 13) {
      car.engineType =
          model->data(model->index(i, 8)).toString(); // engine_type
      car.engineCapacityL =
          model->data(model->index(i, 9)).toDouble();         // engine_capacity
      car.powerHp = model->data(model->index(i, 10)).toInt(); // power_hp
      car.seats = model->data(model->index(i, 11)).toInt();   // seats
      car.quantity = model->data(model->index(i, 12)).toInt(); // quantity
    }
    auto *card = new CarCardWidget(car, currentCurrency_, bmInner_);
    connect(card, &CarCardWidget::rentClicked, this,
            &MainWindow::onCarRentClicked);
    connect(card, &CarCardWidget::bookmarkToggled, this,
            &MainWindow::onBookmarkToggled);
    grid->addWidget(card, row, col, 1, 1, Qt::AlignLeft | Qt::AlignTop);
    col++;
    if (col >= colsPerRow) {
      col = 0;
      row++;
    }
  }
  delete model;
}

void MainWindow::onCarRentClicked(int carId) {
  CarInfo car = Database::instance().getCar(carId, currentCustomerId_);
  if (car.id <= 0)
    return;

  RentalDialog dialog(car, currentCustomerId_, currentCurrency_, this);
  if (dialog.exec() == QDialog::Accepted) {
    loadCars();
    onShowMyRentals();
  }
}

void MainWindow::onBookmarkToggled(int carId, bool bookmarked) {
  Database::instance().setBookmarked(carId, currentCustomerId_, bookmarked);
  if (tabs_->currentIndex() == tabs_->indexOf(bookmarksContainer_)) {
    loadBookmarks();
  }
  // Оптимизация: обновляем только нужную карточку на главной вкладке
  // вместо пересоздания всех карточек
  for (auto *card : carCards_) {
    if (card->carId() == carId) {
      card->updateBookmarkStatus(bookmarked);
      break;
    }
  }
}

void MainWindow::onShowMyRentals() {
  int rentalsIdx = tabs_->indexOf(rentalsWidget_);
  if (rentalsIdx >= 0) {
    tabs_->setCurrentIndex(rentalsIdx);
  }
  if (rentalsModel_) {
    delete rentalsModel_;
    rentalsModel_ = nullptr;
  }
  rentalsModel_ = Database::instance().getCustomerRentals(currentCustomerId_);
  rentalsTable_->setModel(rentalsModel_);
  rentalsTable_->resizeColumnsToContents();
  rentalsTable_->horizontalHeader()->setStretchLastSection(true);
}

void MainWindow::onShowBookmarks() { loadBookmarks(); }

void MainWindow::onSearchChanged() { loadCars(); }

void MainWindow::onCurrencyChanged() {
  currentCurrency_ = currencyBox_->currentText();
  updateCurrencyForAllCards();
}

void MainWindow::updateCurrencyForAllCards() {
  for (auto *card : carCards_) {
    card->updateCurrency(currentCurrency_);
  }
}

void MainWindow::onLogout() {
  currentCustomerId_ = -1;
  currentCustomerName_.clear();
  tabs_->setVisible(false);
  myRentalsBtn_->setVisible(false);
  logoutBtn_->setVisible(false);
  if (rentalsModel_) {
    delete rentalsModel_;
    rentalsModel_ = nullptr;
  }
  QFile::remove(QDir::homePath() + "/.carrental/session.json");
  this->hide();
  showLogin();
}
