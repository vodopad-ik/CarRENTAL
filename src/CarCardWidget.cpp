#include "CarCardWidget.h"

#include "db/CarInfo.h"
#include "exceptions/FileLoadException.h"
#include "utils/CarDetailsFormatter.h"
#include "utils/CarImageLoader.h"
#include "utils/CurrencyConverter.h"

#include <QEvent>
#include <QFrame>
#include <QGraphicsDropShadowEffect>
#include <QHBoxLayout>
#include <QLabel>
#include <QPixmap>
#include <QPushButton>
#include <QToolButton>
#include <QVBoxLayout>

CarCardWidget::CarCardWidget(const CarInfo &car, const QString &currency,
                             QWidget *parent)
    : QWidget(parent), carData_(new CarInfo(car)), currentCurrency_(currency) {
  setMinimumSize(320, 360);
  setMaximumSize(360, 420);

  setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Minimum);
  setStyleSheet("CarCardWidget { background-color: white; border: none; "
                "border-radius: 10px; } "
                "CarCardWidget > QLabel { background-color: transparent; "
                "border: none; }");

  auto *layout = new QVBoxLayout(this);
  layout->setSpacing(6);
  layout->setContentsMargins(12, 12, 12, 12);

  imageLabel_ = new QLabel(this);
  imageLabel_->setFixedHeight(180);
  imageLabel_->setAlignment(Qt::AlignCenter);
  imageLabel_->setStyleSheet(
      "QLabel { background-color: transparent; border: none; }");
  imageLabel_->setScaledContents(false);

  auto *shadow = new QGraphicsDropShadowEffect(this);
  shadow->setBlurRadius(18);
  shadow->setOffset(0, 2);
  shadow->setColor(QColor(0, 0, 0, 80));
  imageLabel_->setGraphicsEffect(shadow);

  QPixmap cardImage;
  try {
    cardImage =
        CarImageLoader::loadCardImage(carData_->imagePath, QSize(320, 180));
    imageLabel_->setPixmap(cardImage);
    imageLabel_->setText("");
  } catch (const FileLoadException &) {
    imageLabel_->setText("🚗");
    imageLabel_->setStyleSheet(
        "QLabel { background-color: #e0e0e0; border: none; color: #999; "
        "font-size: 40px; }");
  }
  layout->addWidget(imageLabel_, 0, Qt::AlignHCenter);

  imageLabel_->setMouseTracking(true);
  imageLabel_->installEventFilter(this);
  setMouseTracking(true);

  const QString carName = CarDetailsFormatter::getName(*carData_);
  nameLabel_ = new QLabel(carName, this);
  nameLabel_->setStyleSheet(
      "QLabel { background-color: transparent; border: none; padding: 0px; "
      "font-size: 16px; font-weight: bold; color: #333; }");
  nameLabel_->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
  nameLabel_->setWordWrap(false);
  nameLabel_->setTextFormat(Qt::PlainText);
  layout->addWidget(nameLabel_);

  priceLabel_ = new QLabel(this);
  updatePriceDisplay();
  priceLabel_->setStyleSheet(
      "QLabel { background-color: transparent; border: none; padding: 0px; "
      "font-size: 14px; color: #4CAF50; font-weight: bold; }");
  priceLabel_->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
  priceLabel_->setWordWrap(false);
  priceLabel_->setTextFormat(Qt::PlainText);
  layout->addWidget(priceLabel_);

  descLabel_ = new QLabel(carData_->description, this);
  descLabel_->setWordWrap(true);
  descLabel_->setMaximumHeight(40);
  descLabel_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
  descLabel_->setStyleSheet(
      "QLabel { background-color: transparent; border: none; padding-top: 2px; "
      "font-size: 12px; color: #455A64; font-weight: 500; }");
  descLabel_->setAlignment(Qt::AlignLeft | Qt::AlignTop);
  descLabel_->setTextFormat(Qt::PlainText);
  layout->addWidget(descLabel_);

  detailsPopup_ = new QFrame(this);
  detailsPopup_->setWindowFlags(Qt::Widget | Qt::FramelessWindowHint);
  detailsPopup_->setStyleSheet("QFrame { background-color: black; "
                               "border: none; border-radius: 8px; }");
  detailsPopup_->hide();
  detailsPopup_->setAttribute(Qt::WA_TransparentForMouseEvents);

  auto *popupLayout = new QVBoxLayout(detailsPopup_);
  popupLayout->setContentsMargins(12, 12, 12, 12);
  popupLayout->setSpacing(4);

  detailsLabel_ = new QLabel(detailsPopup_);
  detailsLabel_->setWordWrap(true);
  detailsLabel_->setStyleSheet(
      "QLabel { background: transparent; border: none; "
      "font-size: 12px; color: #E0E0E0; font-weight: 600; "
      "padding: 0px; margin: 0px; line-height: 1.4; }");
  detailsLabel_->setAlignment(Qt::AlignLeft | Qt::AlignTop);
  popupLayout->addWidget(detailsLabel_);

  auto *popupShadow = new QGraphicsDropShadowEffect(detailsPopup_);
  popupShadow->setBlurRadius(12);
  popupShadow->setOffset(0, 2);
  popupShadow->setColor(QColor(0, 0, 0, 100));
  detailsPopup_->setGraphicsEffect(popupShadow);

  auto *btnLayout = new QHBoxLayout();
  btnLayout->setContentsMargins(0, -20, 0, 0);

  rentBtn_ = new QPushButton("Арендовать", this);
  rentBtn_->setStyleSheet(
      "QPushButton { background-color: #2196F3; color: white; "
      "padding: 10px 14px; border-radius: 6px; font-size: 14px; }"
      "QPushButton:hover { background-color: #1976D2; }");
  rentBtn_->setMinimumHeight(38);
  btnLayout->addWidget(rentBtn_);

  bookmarkBtn_ = new QToolButton(this);
  bookmarkBtn_->setMinimumSize(38, 38);
  bookmarkBtn_->setMaximumSize(38, 38);
  updateBookmarkButton();
  btnLayout->addWidget(bookmarkBtn_);

  layout->addLayout(btnLayout);

  connect(rentBtn_, &QPushButton::clicked,
          [this]() { emit rentClicked(carData_->id); });

  connect(bookmarkBtn_, &QToolButton::clicked, [this]() {
    carData_->bookmarked = !carData_->bookmarked;
    updateBookmarkButton();
    emit bookmarkToggled(carData_->id, carData_->bookmarked);
  });
}

void CarCardWidget::updateCurrency(const QString &currency) {
  currentCurrency_ = currency;
  updatePriceDisplay();
}

int CarCardWidget::getCarId() const { return carData_ ? carData_->id : -1; }

void CarCardWidget::updateBookmarkStatus(bool bookmarked) {
  if (!carData_)
    return;
  carData_->bookmarked = bookmarked;
  updateBookmarkButton();
}

void CarCardWidget::updateBookmarkButton() {
  if (!carData_ || !bookmarkBtn_)
    return;

  bookmarkBtn_->setText(carData_->bookmarked ? "★" : "☆");
  bookmarkBtn_->setToolTip(carData_->bookmarked ? "Удалить из закладок"
                                                : "В закладки");
  bookmarkBtn_->setStyleSheet(
      QString("QToolButton { background-color: %1; border: none; "
              "border-radius: 6px; padding: 6px; font-size: 20px; color: %2; }"
              "QToolButton:hover { background-color: %3; }")
          .arg(carData_->bookmarked ? "#FFC107" : "#E0E0E0")
          .arg(carData_->bookmarked ? "#FF6F00" : "#757575")
          .arg(carData_->bookmarked ? "#FFB300" : "#D0D0D0"));
}

void CarCardWidget::updatePriceDisplay() {
  const auto &converter = CurrencyConverter::instance();
  auto currency = CurrencyConverter::fromString(currentCurrency_);
  double price = converter.fromBase(carData_->pricePerDay, currency);
  QString symbol = converter.symbol(currency);

  QString priceText = QString::number(price, 'f', 2) + " " + symbol + "/день";
  priceLabel_->setText(priceText);
}

bool CarCardWidget::eventFilter(QObject *obj, QEvent *event) {
  if (obj == imageLabel_) {
    if (event->type() == QEvent::Enter) {
      showDetailsTooltip();
      return true;
    } else if (event->type() == QEvent::Leave) {
      hideDetailsTooltip();
      return true;
    }
  }
  return QWidget::eventFilter(obj, event);
}

void CarCardWidget::showDetailsTooltip() {
  const QString detailsText = CarDetailsFormatter::tooltipText(*carData_);
  if (detailsText.isEmpty()) {
    return;
  }

  detailsLabel_->setText(detailsText.trimmed());

  int margin = 12;
  int cardWidth = width();

  int imageX = imageLabel_->x();
  int imageY = imageLabel_->y();
  int imageWidth = imageLabel_->width();
  int imageHeight = imageLabel_->height();

  int popupWidth = 180;
  detailsLabel_->setFixedWidth(popupWidth - 28);
  detailsLabel_->adjustSize();

  int labelHeight = detailsLabel_->height();
  int popupHeight = labelHeight + 32;

  int popupX = imageX + imageWidth + 8;
  int popupY = imageY + imageHeight - popupHeight - 8;

  if (popupX + popupWidth > cardWidth - margin) {
    popupX = cardWidth - popupWidth - margin;
  }
  if (popupY < imageY) {
    popupY = imageY + imageHeight - popupHeight - 5;
  }

  detailsPopup_->setGeometry(popupX, popupY, popupWidth, popupHeight);
  detailsPopup_->raise();
  detailsPopup_->show();
  detailsPopup_->update();
}

void CarCardWidget::hideDetailsTooltip() { detailsPopup_->hide(); }
