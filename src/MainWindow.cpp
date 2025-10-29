#include "MainWindow.h"
#include "CarCardWidget.h"
#include "LoginDialog.h"
#include "RentalDialog.h"
#include "db/Database.h"

#include <QApplication>
#include <QComboBox>
#include <QCursor>
#include <QDialog>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QList>
#include <QMessageBox>
#include <QPushButton>
#include <QScrollArea>
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
  showLogin();
}

MainWindow::~MainWindow() {
  if (rentalsModel_) {
    delete rentalsModel_;
  }
}

void MainWindow::setupUI() {
  centralWidget_ = new QWidget(this);
  setCentralWidget(centralWidget_);

  // Устанавливаем стандартный курсор для главного окна
  setCursor(QCursor(Qt::ArrowCursor));

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
  myRentalsBtn_->setStyleSheet(
      "QPushButton { background-color: white; color: #2196F3; padding: 8px "
      "15px; border-radius: 5px; }");
  headerLayout->addWidget(myRentalsBtn_);

  // Currency selector
  currencyBox_ = new QComboBox(header);
  currencyBox_->addItem("USD");
  currencyBox_->addItem("EUR");
  currencyBox_->addItem("BYN");
  headerLayout->addWidget(currencyBox_);

  logoutBtn_ = new QPushButton("Выйти", header);
  logoutBtn_->setStyleSheet(
      "QPushButton { background-color: white; color: #2196F3; padding: 8px "
      "15px; border-radius: 5px; }");
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
      "border-radius: 5px; }");
  searchLayout->addWidget(searchEdit_);

  refreshBtn_ = new QPushButton("Обновить");
  refreshBtn_->setStyleSheet(
      "QPushButton { padding: 10px 20px; background-color: #4CAF50; color: "
      "white; border-radius: 5px; }");
  searchLayout->addWidget(refreshBtn_);

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

  scrollArea_->setWidget(carsContainer_);
  carsLayout->addWidget(scrollArea_);

  tabs_->addTab(carsTab, "Автомобили");

  // My Rentals tab
  rentalsWidget_ = new QWidget();
  auto *rentalsLayout = new QVBoxLayout(rentalsWidget_);
  rentalsTable_ = new QTableView();
  rentalsTable_->setStyleSheet("QTableView { border: none; }");
  rentalsLayout->addWidget(rentalsTable_);
  tabs_->addTab(rentalsWidget_, "Мои аренды");

  mainLayout->addWidget(tabs_);

  // Connections
  connect(refreshBtn_, &QPushButton::clicked, this, &MainWindow::onRefreshCars);
  connect(currencyBox_, &QComboBox::currentTextChanged, this,
          &MainWindow::onCurrencyChanged);
  connect(myRentalsBtn_, &QPushButton::clicked, this,
          &MainWindow::onShowMyRentals);
  connect(logoutBtn_, &QPushButton::clicked, this, &MainWindow::onLogout);
  connect(searchEdit_, &QLineEdit::textChanged, this,
          &MainWindow::onSearchChanged);

  // Initially hide tabs until login
  tabs_->setVisible(false);
  myRentalsBtn_->setVisible(false);
  logoutBtn_->setVisible(false);
}

void MainWindow::showLogin() {
  LoginDialog dialog(this);
  if (dialog.exec() == QDialog::Accepted) {
    currentCustomerId_ = dialog.customerId();
    currentCustomerName_ = dialog.customerName();
    welcomeLabel_->setText(
        QString("Добро пожаловать, %1!").arg(currentCustomerName_));
    tabs_->setVisible(true);
    myRentalsBtn_->setVisible(true);
    logoutBtn_->setVisible(true);
    loadCars();
  } else {
    QApplication::quit();
  }
}

void MainWindow::loadCars() {
  // Clear existing cards
  for (auto *card : carCards_) {
    card->deleteLater();
  }
  carCards_.clear();

  auto *model = Database::instance().getAvailableCars();
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
    car.pricePerHour = model->data(model->index(i, 4)).toDouble();
    car.description = model->data(model->index(i, 5)).toString();
    car.available = model->data(model->index(i, 6)).toBool();
    car.imagePath = model->data(model->index(i, 7)).toString();

    QString carText =
        (car.brand + " " + car.model + " " + QString::number(car.year))
            .toLower();
    if (!searchText.isEmpty() && !carText.contains(searchText)) {
      continue;
    }

    auto *card = new CarCardWidget(car, currentCurrency_, carsContainer_);
    connect(card, &CarCardWidget::rentClicked, this,
            &MainWindow::onCarRentClicked);
    gridLayout->addWidget(card, row, col);
    carCards_.append(card);

    col++;
    if (col >= colsPerRow) {
      col = 0;
      row++;
    }
  }

  delete model;
}

void MainWindow::onCarRentClicked(int carId) {
  CarInfo car = Database::instance().getCar(carId);
  if (car.id <= 0)
    return;

  RentalDialog dialog(car, currentCustomerId_, currentCurrency_, this);
  if (dialog.exec() == QDialog::Accepted) {
    loadCars();
    onShowMyRentals();
  }
}

void MainWindow::onRefreshCars() { loadCars(); }

void MainWindow::onShowMyRentals() {
  tabs_->setCurrentIndex(1);
  if (rentalsModel_) {
    delete rentalsModel_;
    rentalsModel_ = nullptr;
  }
  rentalsModel_ = Database::instance().getCustomerRentals(currentCustomerId_);
  rentalsTable_->setModel(rentalsModel_);
  rentalsTable_->resizeColumnsToContents();
  rentalsTable_->horizontalHeader()->setStretchLastSection(true);
}

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
  showLogin();
}
