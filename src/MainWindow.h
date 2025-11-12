#pragma once

#include "controllers/CarsCatalogController.h"

#include <QLabel>
#include <QMainWindow>
#include <QScrollArea>
#include <QWidget>

#include <memory>

class LoginDialog;
class CarCardWidget;
class QLineEdit;
class QPushButton;
class QTabWidget;
class QTableView;
class QComboBox;
class QSpinBox;
class QDoubleSpinBox;
class RentalsModel;
class CarCardsView;

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow() override;

private slots:
  void onCarRentClicked(int carId);
  void onBookmarkToggled(int carId, bool bookmarked);
  void onShowMyRentals();
  void onShowBookmarks();
  void onCurrencyChanged();
  void onLogout();

private:
  void setupUI();
  void loadCars();
  void loadBookmarks();
  void showLogin();
  void updateCurrencyForAllCards();
  void tryAutoLogin();
  void connectFilters();
  CatalogFilters currentFilters() const;

  int currentCustomerId_ = -1;
  QString currentCustomerName_;
  QString currentCurrency_;


  QWidget *centralWidget_;
  QTabWidget *tabs_;
  QScrollArea *scrollArea_;
  QWidget *carsContainer_;
  QLineEdit *searchEdit_;
  QComboBox *currencyBox_;

  QComboBox *engineTypeFilter_;
  QSpinBox *seatsMinFilter_;
  QSpinBox *powerMinFilter_;
  QDoubleSpinBox *capacityMinFilter_;
  QPushButton *myRentalsBtn_;
  QPushButton *logoutBtn_;
  QLabel *welcomeLabel_;
  QTableView *rentalsTable_;
  QWidget *rentalsWidget_;
  QWidget *bookmarksContainer_;
  QWidget *bmInner_;

  std::unique_ptr<CarCardsView> carsView_;
  std::unique_ptr<CarCardsView> bookmarksView_;
  std::unique_ptr<CarsCatalogController> catalogController_;
  RentalsModel *rentalsModel_ = nullptr;
};
