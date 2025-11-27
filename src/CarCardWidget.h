#pragma once

#include <QFrame>
#include <QLabel>
#include <QPushButton>
#include <QToolButton>
#include <QWidget>

class QVBoxLayout;
struct CarInfo;

class CarCardWidget : public QWidget {
  Q_OBJECT

public:
  explicit CarCardWidget(const CarInfo &car, const QString &currency = "USD",
                         QWidget *parent = nullptr);
  void updateCurrency(const QString &currency);
  void updateBookmarkStatus(bool bookmarked);
  int getCarId() const;
  bool eventFilter(QObject *obj, QEvent *event) override;

signals:
  void rentClicked(int carId);
  void bookmarkToggled(int carId, bool bookmarked);

private:
  void setupImageLabel(QVBoxLayout *layout);
  void setupNameLabel(QVBoxLayout *layout);
  void setupPriceLabel(QVBoxLayout *layout);
  void setupDescLabel(QVBoxLayout *layout);
  void setupDetailsPopup();
  void setupButtons(QVBoxLayout *layout);
  void connectSignals();
  void updatePriceDisplay();
  void showDetailsTooltip();
  void hideDetailsTooltip();
  void updateBookmarkButton();

  CarInfo *carData_;
  QString currentCurrency_;
  QLabel *imageLabel_;
  QLabel *nameLabel_;
  QLabel *priceLabel_;
  QLabel *descLabel_;
  QFrame *detailsPopup_;
  QLabel *detailsLabel_;
  QPushButton *rentBtn_;
  QToolButton *bookmarkBtn_;
};
