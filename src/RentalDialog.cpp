#include "RentalDialog.h"
#include "db/Database.h"
#include "utils/CurrencyConverter.h"

#include <QDate>
#include <QFormLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>

RentalDialog::RentalDialog(const CarInfo &car, int customerId,
                           const QString &currency, QWidget *parent)
    : QDialog(parent), customerId_(customerId), currentCurrency_(currency) {
  car_ = new CarInfo(car);
  setWindowTitle("Оформление аренды");
  setModal(true);
  resize(350, 250);

  auto *layout = new QVBoxLayout(this);

  auto *carLabel = new QLabel(
      QString("%1 %2 (%3)")
          .arg(car_->brand, car_->model, QString::number(car_->year)),
      this);
  carLabel->setStyleSheet(
      "font-size: 18px; font-weight: bold; margin-bottom: 15px;");
  layout->addWidget(carLabel);

  auto *formLayout = new QFormLayout();

  startDateEdit_ = new QDateTimeEdit(QDateTime::currentDateTime(), this);
  startDateEdit_->setMinimumDateTime(QDateTime::currentDateTime());
  startDateEdit_->setCalendarPopup(true);
  formLayout->addRow("Дата начала:", startDateEdit_);

  endDateEdit_ =
      new QDateTimeEdit(QDateTime::currentDateTime().addSecs(3600), this);
  endDateEdit_->setMinimumDateTime(QDateTime::currentDateTime().addSecs(3600));
  endDateEdit_->setCalendarPopup(true);
  formLayout->addRow("Дата окончания:", endDateEdit_);

  layout->addLayout(formLayout);

  totalPriceLabel_ = new QLabel("Итого: 0", this);
  totalPriceLabel_->setStyleSheet(
      "font-size: 16px; font-weight: bold; color: #4CAF50;");
  layout->addWidget(totalPriceLabel_);

  connect(startDateEdit_, &QDateTimeEdit::dateTimeChanged, this,
          &RentalDialog::updatePrice);
  connect(endDateEdit_, &QDateTimeEdit::dateTimeChanged, this,
          &RentalDialog::updatePrice);
  updatePrice();

  auto *btnLayout = new QHBoxLayout();
  auto *cancelBtn = new QPushButton("Отмена", this);
  auto *rentBtn = new QPushButton("Оформить аренду", this);
  rentBtn->setStyleSheet("QPushButton { background-color: #4CAF50; color: "
                         "white; padding: 10px; }");

  btnLayout->addWidget(cancelBtn);
  btnLayout->addWidget(rentBtn);
  layout->addLayout(btnLayout);

  connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
  connect(rentBtn, &QPushButton::clicked, this, &RentalDialog::onRent);
}

void RentalDialog::updatePrice() {
  QDateTime start = startDateEdit_->dateTime();
  QDateTime end = endDateEdit_->dateTime();

  if (end < start) {
    endDateEdit_->setDateTime(start.addSecs(3600));
    end = start.addSecs(3600);
  }

  qint64 hours = start.secsTo(end) / 3600;
  if (hours <= 0)
    hours = 1;

  auto &converter = CurrencyConverter::instance();
  auto currency = CurrencyConverter::fromString(currentCurrency_);
  double pricePerHour = converter.fromBase(car_->pricePerHour, currency);
  double total = hours * pricePerHour;
  QString symbol = converter.symbol(currency);

  totalPriceLabel_->setText(QString("Итого: %1 %2 (%3 ч)")
                                .arg(total, 0, 'f', 2)
                                .arg(symbol)
                                .arg(hours));
}

void RentalDialog::onRent() {
  QDateTime start = startDateEdit_->dateTime();
  QDateTime end = endDateEdit_->dateTime();

  if (end <= start) {
    QMessageBox::warning(this, "Ошибка",
                         "Дата окончания должна быть позже даты начала");
    return;
  }

  if (!Database::instance().isCarAvailable(car_->id, start.date(),
                                           end.date())) {
    QMessageBox::warning(this, "Недоступно",
                         "Этот автомобиль уже арендован на выбранные даты");
    return;
  }

  qint64 hours = start.secsTo(end) / 3600;
  if (hours <= 0)
    hours = 1;
  double total = hours * car_->pricePerHour;

  int rentalId = Database::instance().createRental(
      car_->id, customerId_, start.date(), end.date(), total);
  if (rentalId > 0) {
    QMessageBox::information(
        this, "Успешно",
        QString("Аренда оформлена! Номер заказа: %1").arg(rentalId));
    accept();
  } else {
    QMessageBox::warning(this, "Ошибка", "Не удалось оформить аренду");
  }
}
