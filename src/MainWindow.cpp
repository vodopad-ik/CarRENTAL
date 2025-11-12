#include "MainWindow.h"
#include "CarCardWidget.h"
#include "LoginDialog.h"
#include "RentalDialog.h"
#include "RentalsModel.h"
#include "controllers/CarsCatalogController.h"
#include "db/Database.h"
#include "utils/SessionManager.h"
#include "views/CarCardsView.h"

#include <QApplication>
#include <QComboBox>
#include <QDialog>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QHeaderView>
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

  catalogController_ = std::make_unique<CarsCatalogController>();

  if (!Database::instance().initialize()) {
    QMessageBox::critical(this, "Ошибка",
                          "Не удалось инициализировать базу данных");
  }

  setupUI();
  tryAutoLogin();
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

  auto *header = new QWidget(this);
  header->setStyleSheet("background-color: #2196F3; padding: 15px;");
  auto *headerLayout = new QHBoxLayout(header);

  welcomeLabel_ = new QLabel("Добро пожаловать", header);
  welcomeLabel_->setStyleSheet(
      "color: white; font-size: 18px; font-weight: bold;");
  headerLayout->addWidget(welcomeLabel_);

  headerLayout->addStretch();

  myRentalsBtn_ = new QPushButton("Мои аренды", header);

  const QString headerBtnStyle =
      "QPushButton { background-color: white; color: #2196F3; padding: 8px "
      "16px; border-radius: 6px; }"
      "QPushButton:hover { filter: brightness(0.95); }"
      "QPushButton:pressed { transform: translateY(1px); }";
  myRentalsBtn_->setStyleSheet(headerBtnStyle);
  myRentalsBtn_->setIcon(style()->standardIcon(QStyle::SP_FileDialogListView));
  myRentalsBtn_->setMinimumHeight(36);
  headerLayout->addWidget(myRentalsBtn_);

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

  tabs_ = new QTabWidget(this);
  tabs_->setStyleSheet("QTabWidget::pane { border: none; }");

  auto *carsTab = new QWidget();
  auto *carsLayout = new QVBoxLayout(carsTab);

  auto *searchLayout = new QHBoxLayout();
  searchEdit_ = new QLineEdit();
  searchEdit_->setPlaceholderText("Поиск по марке, модели...");
  searchEdit_->setStyleSheet(
      "QLineEdit { padding: 10px; font-size: 14px; border: 2px solid #e0e0e0; "
      "border-radius: 5px; }"
      "QLineEdit:focus { border-color: #2196F3; }");
  searchLayout->addWidget(searchEdit_);

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

  scrollArea_ = new QScrollArea();
  scrollArea_->setWidgetResizable(true);
  scrollArea_->setStyleSheet(
      "QScrollArea { border: none; background-color: #f5f5f5; }");

  carsContainer_ = new QWidget();
  carsContainer_->setStyleSheet("background-color: #f5f5f5;");
  auto *gridLayout = new QGridLayout(carsContainer_);
  gridLayout->setSpacing(20);
  gridLayout->setContentsMargins(20, 20, 20, 20);

  for (int i = 0; i < 3; ++i) {
    gridLayout->setColumnStretch(i, 0);
    gridLayout->setColumnMinimumWidth(i, 320);
  }

  scrollArea_->setWidget(carsContainer_);
  carsLayout->addWidget(scrollArea_);

  tabs_->addTab(carsTab, "Автомобили");

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

  for (int i = 0; i < 3; ++i) {
    bmGrid->setColumnStretch(i, 0);
    bmGrid->setColumnMinimumWidth(i, 320);
  }
  bmScroll->setWidget(bmInner_);
  bmLayoutOuter->addWidget(bmScroll);
  tabs_->addTab(bookmarksContainer_, "Закладки");

  rentalsWidget_ = new QWidget();
  auto *rentalsLayout = new QVBoxLayout(rentalsWidget_);
  rentalsTable_ = new QTableView();
  rentalsTable_->setStyleSheet("QTableView { border: none; }");
  rentalsLayout->addWidget(rentalsTable_);
  tabs_->addTab(rentalsWidget_, "Мои аренды");

  mainLayout->addWidget(tabs_);

  carsView_ = std::make_unique<CarCardsView>(carsContainer_, 3);
  carsView_->setCurrency(currentCurrency_);
  bookmarksView_ = std::make_unique<CarCardsView>(bmInner_, 3);
  bookmarksView_->setCurrency(currentCurrency_);

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
  connectFilters();
}

void MainWindow::tryAutoLogin() {
  SessionManager::SessionData session;
  if (SessionManager::instance().loadSession(session) &&
      session.customerId > 0) {
    currentCustomerId_ = session.customerId;
    currentCustomerName_ = session.name;
    welcomeLabel_->setText(
        QString("Добро пожаловать, %1!").arg(currentCustomerName_));
    tabs_->setVisible(true);
    myRentalsBtn_->setVisible(true);
    logoutBtn_->setVisible(true);
    loadCars();
    Database::instance().updateExpiredRentals();
    if (rentalsModel_) {
      delete rentalsModel_;
      rentalsModel_ = nullptr;
    }
    rentalsModel_ = new RentalsModel(this);
    rentalsModel_->setCurrency(currentCurrency_);
    rentalsModel_->refresh(currentCustomerId_);
    rentalsTable_->setModel(rentalsModel_);
    rentalsTable_->resizeColumnsToContents();
    rentalsTable_->horizontalHeader()->setStretchLastSection(true);
    tabs_->setCurrentIndex(0);
    this->show();
  } else {
    showLogin();
  }
}

void MainWindow::showLogin() {
  this->hide();
  LoginDialog dialog(this);
  if (dialog.exec() == QDialog::Accepted) {
    currentCustomerId_ = dialog.customerId();
    currentCustomerName_ = dialog.customerName();
    welcomeLabel_->setText(
        QString("Добро пожаловать, %1!").arg(currentCustomerName_));
    SessionManager::instance().saveSession(currentCustomerId_,
                                           currentCustomerName_);
    tabs_->setVisible(true);
    myRentalsBtn_->setVisible(true);
    logoutBtn_->setVisible(true);
    loadCars();

    Database::instance().updateExpiredRentals();
    if (rentalsModel_) {
      delete rentalsModel_;
      rentalsModel_ = nullptr;
    }
    rentalsModel_ = new RentalsModel(this);
    rentalsModel_->setCurrency(currentCurrency_);
    rentalsModel_->refresh(currentCustomerId_);
    rentalsTable_->setModel(rentalsModel_);
    rentalsTable_->resizeColumnsToContents();
    rentalsTable_->horizontalHeader()->setStretchLastSection(true);

    tabs_->setCurrentIndex(0);
    this->show();
  } else {

    QApplication::quit();
  }
}

void MainWindow::loadCars() {
  if (!catalogController_)
    catalogController_ = std::make_unique<CarsCatalogController>();

  if (currentCustomerId_ <= 0)
    return;

  catalogController_->setFilters(currentFilters());
  const QList<CarInfo> cars =
      catalogController_->loadAvailable(currentCustomerId_);

  carsView_->setCurrency(currentCurrency_);
  carsView_->showCars(cars, [this](CarCardWidget *card) {
    connect(card, &CarCardWidget::rentClicked, this,
            &MainWindow::onCarRentClicked);
    connect(card, &CarCardWidget::bookmarkToggled, this,
            &MainWindow::onBookmarkToggled);
  });
}

void MainWindow::loadBookmarks() {
  if (!catalogController_)
    catalogController_ = std::make_unique<CarsCatalogController>();

  if (currentCustomerId_ <= 0)
    return;

  catalogController_->setFilters(currentFilters());
  const QList<CarInfo> cars =
      catalogController_->loadBookmarked(currentCustomerId_);

  bookmarksView_->setCurrency(currentCurrency_);
  bookmarksView_->showCars(cars, [this](CarCardWidget *card) {
    connect(card, &CarCardWidget::rentClicked, this,
            &MainWindow::onCarRentClicked);
    connect(card, &CarCardWidget::bookmarkToggled, this,
            &MainWindow::onBookmarkToggled);
  });
}

void MainWindow::onCarRentClicked(int carId) {
  CarInfo car = Database::instance().getCar(carId, currentCustomerId_);
  if (car.id <= 0)
    return;

  RentalDialog dialog(car, currentCustomerId_, currentCurrency_, this);
  if (dialog.exec() == QDialog::Accepted) {
    loadCars();

    Database::instance().updateExpiredRentals();
    if (rentalsModel_) {
      rentalsModel_->refresh(currentCustomerId_);
    } else {
      rentalsModel_ = new RentalsModel(this);
      rentalsModel_->setCurrency(currentCurrency_);
      rentalsModel_->refresh(currentCustomerId_);
      rentalsTable_->setModel(rentalsModel_);
    }
    onShowMyRentals();
  }
}

void MainWindow::onBookmarkToggled(int carId, bool bookmarked) {
  Database::instance().setBookmarked(carId, currentCustomerId_, bookmarked);
  carsView_->forEachCard([&](CarCardWidget *card) {
    if (card->carId() == carId)
      card->updateBookmarkStatus(bookmarked);
  });

  loadBookmarks();
}

void MainWindow::onShowMyRentals() {
  int rentalsIdx = tabs_->indexOf(rentalsWidget_);
  if (rentalsIdx >= 0) {
    tabs_->setCurrentIndex(rentalsIdx);
  }

  Database::instance().updateExpiredRentals();
  if (rentalsModel_) {
    delete rentalsModel_;
    rentalsModel_ = nullptr;
  }
  rentalsModel_ = new RentalsModel(this);
  rentalsModel_->setCurrency(currentCurrency_);
  rentalsModel_->refresh(currentCustomerId_);
  rentalsTable_->setModel(rentalsModel_);
  rentalsTable_->resizeColumnsToContents();
  rentalsTable_->horizontalHeader()->setStretchLastSection(true);
}

void MainWindow::onShowBookmarks() { loadBookmarks(); }

void MainWindow::onCurrencyChanged() {
  currentCurrency_ = currencyBox_->currentText();
  updateCurrencyForAllCards();

  if (rentalsModel_) {
    rentalsModel_->setCurrency(currentCurrency_);
  }
}

void MainWindow::updateCurrencyForAllCards() {
  if (carsView_)
    carsView_->setCurrency(currentCurrency_);
  if (bookmarksView_)
    bookmarksView_->setCurrency(currentCurrency_);
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
  SessionManager::instance().clearSession();
  this->hide();
  showLogin();
}
void MainWindow::connectFilters() {
  auto applyFilters = [this]() {
    loadCars();
    if (tabs_->currentWidget() == bookmarksContainer_)
      loadBookmarks();
  };

  connect(searchEdit_, &QLineEdit::textChanged, this,
          [applyFilters](const QString &) { applyFilters(); });
  connect(engineTypeFilter_, &QComboBox::currentTextChanged, this,
          [applyFilters](const QString &) { applyFilters(); });
  connect(seatsMinFilter_, qOverload<int>(&QSpinBox::valueChanged), this,
          [applyFilters](int) { applyFilters(); });
  connect(powerMinFilter_, qOverload<int>(&QSpinBox::valueChanged), this,
          [applyFilters](int) { applyFilters(); });
  connect(capacityMinFilter_, qOverload<double>(&QDoubleSpinBox::valueChanged),
          this, [applyFilters](double) { applyFilters(); });
}

CatalogFilters MainWindow::currentFilters() const {
  CatalogFilters getFilters;
  getFilters.searchText = searchEdit_->text();
  getFilters.engineType = engineTypeFilter_->currentText();
  getFilters.seatsMin = seatsMinFilter_->value();
  getFilters.powerMin = powerMinFilter_->value();
  getFilters.capacityMin = capacityMinFilter_->value();
  return getFilters;
}

