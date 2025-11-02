#pragma once

#include <QFrame>
#include <QLabel>
#include <QPushButton>
#include <QToolButton>
#include <QWidget>

struct CarInfo;

class CarCardWidget : public QWidget {
  Q_OBJECT

public:
  explicit CarCardWidget(const CarInfo &car, const QString &currency = "USD",
                         QWidget *parent = nullptr);
  void updateCurrency(const QString &currency);
  void updateBookmarkStatus(bool bookmarked);
  int carId() const;
  bool eventFilter(QObject *obj, QEvent *event) override;

signals:
  void rentClicked(int carId);
  void bookmarkToggled(int carId, bool bookmarked);

private:
  void updatePriceDisplay();
  void showDetailsTooltip();
  void hideDetailsTooltip();

  CarInfo *carData_;
  QString currentCurrency_;
  QString shortDescription_;
  QLabel *imageLabel_;
  QLabel *nameLabel_;
  QLabel *priceLabel_;
  QLabel *descLabel_;
  QFrame *detailsPopup_;
  QLabel *detailsLabel_;
  QPushButton *rentBtn_;
  QToolButton *bookmarkBtn_;
};
