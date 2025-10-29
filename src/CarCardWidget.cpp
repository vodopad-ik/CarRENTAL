#include "CarCardWidget.h"
#include "db/Database.h"
#include "utils/CurrencyConverter.h"
#include <QCursor>
#include <QHBoxLayout>
#include <QPixmap>
#include <QVBoxLayout>

CarCardWidget::CarCardWidget(const CarInfo &car, const QString &currency,
                             QWidget *parent)
    : QWidget(parent), currentCurrency_(currency) {
  carData_ = new CarInfo(car);
  setMinimumSize(280, 200);
  setMaximumSize(320, 250);
  setStyleSheet(
      "CarCardWidget { background-color: white; border: 2px solid #e0e0e0; "
      "border-radius: 10px; } "
      "CarCardWidget > QLabel { background-color: transparent; border: none; "
      "}");

  // Устанавливаем стандартный курсор для карточки
  setCursor(QCursor(Qt::ArrowCursor));

  auto *layout = new QVBoxLayout(this);
  layout->setSpacing(8);
  layout->setContentsMargins(15, 15, 15, 15);

  // Image - уменьшенный размер
  imageLabel_ = new QLabel(this);
  imageLabel_->setFixedHeight(80);
  imageLabel_->setFixedWidth(240);
  imageLabel_->setAlignment(Qt::AlignCenter);
  imageLabel_->setStyleSheet(
      "QLabel { background-color: #f5f5f5; border-radius: 5px; }");
  imageLabel_->setScaledContents(false);

  QPixmap pix;
  if (!carData_->imagePath.isEmpty()) {
    pix.load(carData_->imagePath);
  }
  if (pix.isNull()) {
    pix.load(":/images/placeholder.svg");
  }
  if (!pix.isNull()) {
    QPixmap scaled =
        pix.scaled(240, 80, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    imageLabel_->setPixmap(scaled);
  }
  layout->addWidget(imageLabel_, 0, Qt::AlignHCenter);

  QString carName = QString("%1 %2 (%3)")
                        .arg(carData_->brand, carData_->model,
                             QString::number(carData_->year));
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
  descLabel_->setStyleSheet(
      "QLabel { background-color: transparent; border: none; padding: 0px; "
      "font-size: 12px; color: #666; }");
  descLabel_->setAlignment(Qt::AlignLeft | Qt::AlignTop);
  descLabel_->setTextFormat(Qt::PlainText);
  layout->addWidget(descLabel_);

  layout->addStretch();

  rentBtn_ = new QPushButton("Арендовать", this);
  rentBtn_->setStyleSheet(
      "QPushButton { background-color: #2196F3; color: white; "
      "padding: 10px; border-radius: 5px; font-size: 14px; }"
      "QPushButton:hover { background-color: #1976D2; }");
  layout->addWidget(rentBtn_);

  connect(rentBtn_, &QPushButton::clicked,
          [this]() { emit rentClicked(carData_->id); });
}

void CarCardWidget::updateCurrency(const QString &currency) {
  currentCurrency_ = currency;
  updatePriceDisplay();
}

void CarCardWidget::updatePriceDisplay() {
  auto &converter = CurrencyConverter::instance();
  auto currency = CurrencyConverter::fromString(currentCurrency_);
  double price = converter.fromBase(carData_->pricePerHour, currency);
  QString symbol = converter.symbol(currency);

  // Форматируем цену правильно с разделителем тысяч и символом валюты
  QString priceText = QString::number(price, 'f', 2) + " " + symbol + "/час";
  priceLabel_->setText(priceText);
}
