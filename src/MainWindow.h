#pragma once

#include "controllers/CarsCatalogController.h"

#include <QLabel>
#include <QMainWindow>
#include <QScrollArea>
#include <QWidget>

#include <functional>
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

namespace MainWindowImpl {
  struct UIWidgets {
    QWidget *centralWidget_ = nullptr;
    QTabWidget *tabs_ = nullptr;
    QScrollArea *scrollArea_ = nullptr;
    QWidget *carsContainer_ = nullptr;
    QWidget *rentalsWidget_ = nullptr;
    QWidget *bookmarksContainer_ = nullptr;
    QWidget *bmInner_ = nullptr;
  };

  struct UIFilters {
    QLineEdit *searchEdit_ = nullptr;
    QComboBox *engineTypeFilter_ = nullptr;
    QSpinBox *seatsMinFilter_ = nullptr;
    QSpinBox *powerMinFilter_ = nullptr;
    QDoubleSpinBox *capacityMinFilter_ = nullptr;
  };

  struct UIButtons {
    QPushButton *myRentalsBtn_ = nullptr;
    QPushButton *logoutBtn_ = nullptr;
  };
}

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
  void initializeUserSession();
  CatalogFilters currentFilters() const;
  void loadCarsToView(CarCardsView *view,
                      std::function<QList<CarInfo>(int)> loadFunction);

  int currentCustomerId_ = -1;
  QString currentCustomerName_;
  QString currentCurrency_ = "USD";

  MainWindowImpl::UIWidgets ui_;
  MainWindowImpl::UIFilters filters_;
  MainWindowImpl::UIButtons buttons_;

  QLabel *welcomeLabel_ = nullptr;

  QComboBox *currencyBox_ = nullptr;
  QTableView *rentalsTable_ = nullptr;

  std::unique_ptr<CarCardsView> carsView_;
  std::unique_ptr<CarCardsView> bookmarksView_;
  std::unique_ptr<CarsCatalogController> catalogController_;
  RentalsModel *rentalsModel_ = nullptr;
};