#pragma once

#include <QLabel>
#include <QMainWindow>
#include <QScrollArea>
#include <QWidget>

class LoginDialog;
class CarCardWidget;
class QLineEdit;
class QPushButton;
class QTabWidget;
class QTableView;
class QComboBox;
class QSqlQueryModel;

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow() override;

private slots:
  void onCarRentClicked(int carId);
  void onRefreshCars();
  void onShowMyRentals();
  void onSearchChanged();
  void onCurrencyChanged();
  void onLogout();

private:
  void setupUI();
  void loadCars();
  void showLogin();
  void updateCurrencyForAllCards();

  int currentCustomerId_ = -1;
  QString currentCustomerName_;
  QString currentCurrency_;

  // UI elements
  QWidget *centralWidget_;
  QTabWidget *tabs_;
  QScrollArea *scrollArea_;
  QWidget *carsContainer_;
  QLineEdit *searchEdit_;
  QComboBox *currencyBox_;
  QPushButton *refreshBtn_;
  QPushButton *myRentalsBtn_;
  QPushButton *logoutBtn_;
  QLabel *welcomeLabel_;
  QTableView *rentalsTable_;
  QWidget *rentalsWidget_;

  QList<CarCardWidget *> carCards_;
  QSqlQueryModel *rentalsModel_ = nullptr;
};
