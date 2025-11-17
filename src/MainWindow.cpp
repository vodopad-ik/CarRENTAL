#include "MainWindow.h"
#include "CarCardWidget.h"
#include "LoginDialog.h"
#include "RentalDialog.h"
#include "RentalsModel.h"
#include "controllers/CarsCatalogController.h"
#include "db/Database.h"
#include "exceptions/AppException.h"
#include "exceptions/DatabaseException.h"
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

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
  setWindowTitle("Аренда автомобилей");
  resize(1200, 800);

  catalogController_ = std::make_unique<CarsCatalogController>();

  try {
    Database::instance().initialize();
  } catch (const DatabaseException &e) {
    QMessageBox::critical(
        this, "Ошибка базы данных",
        QString("Не удалось инициализировать базу данных:\n%1")
            .arg(e.getMessage()));
    QApplication::quit();
    return;
  } catch (const AppException &e) {
    QMessageBox::critical(
        this, "Ошибка",
        QString("Ошибка при инициализации:\n%1").arg(e.getMessage()));
    QApplication::quit();
    return;
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
  ui_.centralWidget_ = new QWidget(this);
  setCentralWidget(ui_.centralWidget_);

  auto *mainLayout = new QVBoxLayout(ui_.centralWidget_);
  mainLayout->setSpacing(0);
  mainLayout->setContentsMargins(0, 0, 0, 0);

  auto *header = new QWidget(this);
  header->setStyleSheet("background-color: #2196F3; padding: 15px;");
  auto *headerLayout = new QHBoxLayout(header);

  labels_.welcomeLabel_ = new QLabel("Добро пожаловать", header);
  labels_.welcomeLabel_->setStyleSheet(
      "color: white; font-size: 18px; font-weight: bold;");
  headerLayout->addWidget(labels_.welcomeLabel_);

  headerLayout->addStretch();

  buttons_.myRentalsBtn_ = new QPushButton("Мои аренды", header);

  const QString headerBtnStyle =
      "QPushButton { background-color: white; color: #2196F3; padding: 8px "
      "16px; border-radius: 6px; }"
      "QPushButton:hover { filter: brightness(0.95); }"
      "QPushButton:pressed { transform: translateY(1px); }";
  buttons_.myRentalsBtn_->setStyleSheet(headerBtnStyle);
  buttons_.myRentalsBtn_->setIcon(
      style()->standardIcon(QStyle::SP_FileDialogListView));
  buttons_.myRentalsBtn_->setMinimumHeight(36);
  headerLayout->addWidget(buttons_.myRentalsBtn_);

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

  buttons_.logoutBtn_ = new QPushButton("Выйти", header);
  buttons_.logoutBtn_->setStyleSheet(headerBtnStyle);
  buttons_.logoutBtn_->setIcon(
      style()->standardIcon(QStyle::SP_DialogCloseButton));
  buttons_.logoutBtn_->setMinimumHeight(36);
  headerLayout->addWidget(buttons_.logoutBtn_);

  mainLayout->addWidget(header);

  ui_.tabs_ = new QTabWidget(this);
  ui_.tabs_->setStyleSheet("QTabWidget::pane { border: none; }");

  auto *carsTab = new QWidget();
  auto *carsLayout = new QVBoxLayout(carsTab);

  auto *searchLayout = new QHBoxLayout();
  filters_.searchEdit_ = new QLineEdit();
  filters_.searchEdit_->setPlaceholderText("Поиск по марке, модели...");
  filters_.searchEdit_->setStyleSheet(
      "QLineEdit { padding: 10px; font-size: 14px; border: 2px solid #e0e0e0; "
      "border-radius: 5px; }"
      "QLineEdit:focus { border-color: #2196F3; }");
  searchLayout->addWidget(filters_.searchEdit_);

  filters_.engineTypeFilter_ = new QComboBox();
  filters_.engineTypeFilter_->addItem("Любой");
  filters_.engineTypeFilter_->addItem("EV");
  filters_.engineTypeFilter_->addItem("Hybrid");
  filters_.engineTypeFilter_->addItem("ICE");
  filters_.engineTypeFilter_->setMinimumHeight(38);
  searchLayout->addWidget(filters_.engineTypeFilter_);

  filters_.seatsMinFilter_ = new QSpinBox();
  filters_.seatsMinFilter_->setRange(0, 9);
  filters_.seatsMinFilter_->setPrefix("Мест ≥ ");
  filters_.seatsMinFilter_->setMinimumHeight(38);
  searchLayout->addWidget(filters_.seatsMinFilter_);

  filters_.powerMinFilter_ = new QSpinBox();
  filters_.powerMinFilter_->setRange(0, 2000);
  filters_.powerMinFilter_->setPrefix("ЛС ≥ ");
  filters_.powerMinFilter_->setMinimumHeight(38);
  searchLayout->addWidget(filters_.powerMinFilter_);

  filters_.capacityMinFilter_ = new QDoubleSpinBox();
  filters_.capacityMinFilter_->setRange(0.0, 10.0);
  filters_.capacityMinFilter_->setDecimals(1);
  filters_.capacityMinFilter_->setSingleStep(0.1);
  filters_.capacityMinFilter_->setPrefix("Литры ≥ ");
  filters_.capacityMinFilter_->setMinimumHeight(38);
  searchLayout->addWidget(filters_.capacityMinFilter_);

  carsLayout->addLayout(searchLayout);

  ui_.scrollArea_ = new QScrollArea();
  ui_.scrollArea_->setWidgetResizable(true);
  ui_.scrollArea_->setStyleSheet(
      "QScrollArea { border: none; background-color: #f5f5f5; }");

  ui_.carsContainer_ = new QWidget();
  ui_.carsContainer_->setStyleSheet("background-color: #f5f5f5;");
  auto *gridLayout = new QGridLayout(ui_.carsContainer_);
  gridLayout->setSpacing(20);
  gridLayout->setContentsMargins(20, 20, 20, 20);

  for (int i = 0; i < 3; ++i) {
    gridLayout->setColumnStretch(i, 0);
    gridLayout->setColumnMinimumWidth(i, 320);
  }

  ui_.scrollArea_->setWidget(ui_.carsContainer_);
  carsLayout->addWidget(ui_.scrollArea_);

  ui_.tabs_->addTab(carsTab, "Автомобили");

  ui_.bookmarksContainer_ = new QWidget();
  auto *bmLayoutOuter = new QVBoxLayout(ui_.bookmarksContainer_);
  auto *bmScroll = new QScrollArea();
  bmScroll->setWidgetResizable(true);
  bmScroll->setStyleSheet(
      "QScrollArea { border: none; background-color: white; }");
  ui_.bmInner_ = new QWidget();
  ui_.bmInner_->setStyleSheet("background-color: white;");
  auto *bmGrid = new QGridLayout(ui_.bmInner_);
  bmGrid->setSpacing(20);
  bmGrid->setContentsMargins(20, 20, 20, 20);

  for (int i = 0; i < 3; ++i) {
    bmGrid->setColumnStretch(i, 0);
    bmGrid->setColumnMinimumWidth(i, 320);
  }
  bmScroll->setWidget(ui_.bmInner_);
  bmLayoutOuter->addWidget(bmScroll);
  ui_.tabs_->addTab(ui_.bookmarksContainer_, "Закладки");

  ui_.rentalsWidget_ = new QWidget();
  auto *rentalsLayout = new QVBoxLayout(ui_.rentalsWidget_);
  rentalsTable_ = new QTableView();
  rentalsTable_->setStyleSheet("QTableView { border: none; }");
  rentalsLayout->addWidget(rentalsTable_);
  ui_.tabs_->addTab(ui_.rentalsWidget_, "Мои аренды");

  mainLayout->addWidget(ui_.tabs_);

  carsView_ = std::make_unique<CarCardsView>(ui_.carsContainer_, 3);
  carsView_->setCurrency(currentCurrency_);
  bookmarksView_ = std::make_unique<CarCardsView>(ui_.bmInner_, 3);
  bookmarksView_->setCurrency(currentCurrency_);

  connect(currencyBox_, &QComboBox::currentTextChanged, this,
          &MainWindow::onCurrencyChanged);
  connect(buttons_.myRentalsBtn_, &QPushButton::clicked, this,
          &MainWindow::onShowMyRentals);
  connect(ui_.tabs_, &QTabWidget::currentChanged, this, [this](int idx) {
    if (ui_.tabs_->tabText(idx) == "Закладки")
      loadBookmarks();
    else if (ui_.tabs_->tabText(idx) == "Мои аренды")
      onShowMyRentals();
  });
  connect(buttons_.logoutBtn_, &QPushButton::clicked, this,
          &MainWindow::onLogout);
  connectFilters();
}

void MainWindow::tryAutoLogin() {
  SessionManager::SessionData session;
  if (SessionManager::instance().loadSession(session) &&
      session.customerId > 0) {
    currentCustomerId_ = session.customerId;
    currentCustomerName_ = session.name;
    labels_.welcomeLabel_->setText(
        QString("Добро пожаловать, %1!").arg(currentCustomerName_));
    ui_.tabs_->setVisible(true);
    buttons_.myRentalsBtn_->setVisible(true);
    buttons_.logoutBtn_->setVisible(true);
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
    ui_.tabs_->setCurrentIndex(0);
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
    labels_.welcomeLabel_->setText(
        QString("Добро пожаловать, %1!").arg(currentCustomerName_));
    SessionManager::instance().saveSession(currentCustomerId_,
                                           currentCustomerName_);
    ui_.tabs_->setVisible(true);
    buttons_.myRentalsBtn_->setVisible(true);
    buttons_.logoutBtn_->setVisible(true);
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

    ui_.tabs_->setCurrentIndex(0);
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
  carsView_->showCars(cars, [this](const CarCardWidget *card) {
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
  carsView_->forEachCard([carId, bookmarked](CarCardWidget *card) {
    if (card->carId() == carId)
      card->updateBookmarkStatus(bookmarked);
  });

  loadBookmarks();
}

void MainWindow::onShowMyRentals() {
  if (int rentalsIdx = ui_.tabs_->indexOf(ui_.rentalsWidget_);
      rentalsIdx >= 0) {
    ui_.tabs_->setCurrentIndex(rentalsIdx);
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
  ui_.tabs_->setVisible(false);
  buttons_.myRentalsBtn_->setVisible(false);
  buttons_.logoutBtn_->setVisible(false);
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
    if (ui_.tabs_->currentWidget() == ui_.bookmarksContainer_)
      loadBookmarks();
  };

  connect(filters_.searchEdit_, &QLineEdit::textChanged, this,
          [applyFilters](const QString &) { applyFilters(); });
  connect(filters_.engineTypeFilter_, &QComboBox::currentTextChanged, this,
          [applyFilters](const QString &) { applyFilters(); });
  connect(filters_.seatsMinFilter_, qOverload<int>(&QSpinBox::valueChanged),
          this, [applyFilters](int) { applyFilters(); });
  connect(filters_.powerMinFilter_, qOverload<int>(&QSpinBox::valueChanged),
          this, [applyFilters](int) { applyFilters(); });
  connect(filters_.capacityMinFilter_,
          qOverload<double>(&QDoubleSpinBox::valueChanged), this,
          [applyFilters](double) { applyFilters(); });
}

CatalogFilters MainWindow::currentFilters() const {
  CatalogFilters getFilters;
  getFilters.searchText = filters_.searchEdit_->text();
  getFilters.engineType = filters_.engineTypeFilter_->currentText();
  getFilters.seatsMin = filters_.seatsMinFilter_->value();
  getFilters.powerMin = filters_.powerMinFilter_->value();
  getFilters.capacityMin = filters_.capacityMinFilter_->value();
  return getFilters;
}
