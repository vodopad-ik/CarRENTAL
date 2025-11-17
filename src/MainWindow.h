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
  QString currentCurrency_ = "USD";

  struct UIWidgets {
    QWidget *centralWidget_ = nullptr;
    QTabWidget *tabs_ = nullptr;
    QScrollArea *scrollArea_ = nullptr;
    QWidget *carsContainer_ = nullptr;
    QWidget *rentalsWidget_ = nullptr;
    QWidget *bookmarksContainer_ = nullptr;
    QWidget *bmInner_ = nullptr;
  };
  
  UIWidgets ui_;

  struct UIFilters {
    QLineEdit *searchEdit_ = nullptr;
    QComboBox *engineTypeFilter_ = nullptr;
    QSpinBox *seatsMinFilter_ = nullptr;
    QSpinBox *powerMinFilter_ = nullptr;
    QDoubleSpinBox *capacityMinFilter_ = nullptr;
  };
  
  UIFilters filters_;

  struct UIButtons {
    QPushButton *myRentalsBtn_ = nullptr;
    QPushButton *logoutBtn_ = nullptr;
  };
  
  UIButtons buttons_;

  struct UILabels {
    QLabel *welcomeLabel_ = nullptr;
  };
  
  UILabels labels_;

  QComboBox *currencyBox_ = nullptr;
  QTableView *rentalsTable_ = nullptr;

  std::unique_ptr<CarCardsView> carsView_;
  std::unique_ptr<CarCardsView> bookmarksView_;
  std::unique_ptr<CarsCatalogController> catalogController_;
  RentalsModel *rentalsModel_ = nullptr;
};
