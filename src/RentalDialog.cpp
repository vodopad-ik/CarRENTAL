#include "RentalDialog.h"
#include "CustomCalendarWidget.h"
#include "db/Database.h"
#include "utils/CurrencyConverter.h"

#include <QDate>
#include <QFormLayout>
#include <QHBoxLayout>
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


  QString specs;
  if (!car_->engineType.isEmpty())
    specs += QString("Тип двигателя: %1\n").arg(car_->engineType);
  if (car_->engineCapacityL > 0.0)
    specs += QString("Объем двигателя: %1 л\n")
                 .arg(QString::number(car_->engineCapacityL, 'f', 1));
  if (car_->powerHp > 0)
    specs += QString("Мощность: %1 л.с.\n").arg(car_->powerHp);
  if (car_->seats > 0)
    specs += QString("Число мест: %1").arg(car_->seats);
  if (!specs.isEmpty()) {
    specsLabel_ = new QLabel(specs, this);
    specsLabel_->setStyleSheet("color:#555; margin-bottom: 10px;");
    layout->addWidget(specsLabel_);
  }

  auto *formLayout = new QFormLayout();

  QDate maxDate =
      QDate::currentDate().addYears(1);

  startDateEdit_ = new QDateEdit(QDate::currentDate(), this);
  startDateEdit_->setMinimumDate(QDate::currentDate());
  startDateEdit_->setMaximumDate(maxDate);
  startDateEdit_->setCalendarPopup(true);
  startDateEdit_->setDisplayFormat("dd.MM.yyyy");

  auto *startCalendar = new CustomCalendarWidget(car_->id, this);
  startDateEdit_->setCalendarWidget(startCalendar);
  formLayout->addRow("Дата начала:", startDateEdit_);

  endDateEdit_ = new QDateEdit(QDate::currentDate().addDays(1), this);
  endDateEdit_->setMinimumDate(QDate::currentDate().addDays(1));
  endDateEdit_->setMaximumDate(maxDate);
  endDateEdit_->setCalendarPopup(true);
  endDateEdit_->setDisplayFormat("dd.MM.yyyy");

  auto *endCalendar = new CustomCalendarWidget(car_->id, this);
  endDateEdit_->setCalendarWidget(endCalendar);
  formLayout->addRow("Дата окончания:", endDateEdit_);

  layout->addLayout(formLayout);

  totalPriceLabel_ = new QLabel("Итого: 0", this);
  totalPriceLabel_->setStyleSheet(
      "font-size: 16px; font-weight: bold; color: #4CAF50;");
  layout->addWidget(totalPriceLabel_);

  connect(startDateEdit_, &QDateEdit::dateChanged, this,
          &RentalDialog::updatePrice);
  connect(endDateEdit_, &QDateEdit::dateChanged, this,
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
  QDate start = startDateEdit_->date();
  QDate end = endDateEdit_->date();
  QDate currentDate = QDate::currentDate();
  QDate maxDate = currentDate.addYears(1);

  const auto normalized =
      calculator_.normalizeDates(start, end, currentDate, maxDate);

  if (normalized.startClamped) {
    QMessageBox::warning(
        this, "Предупреждение",
        QString("Дата начала не может быть позже %1 (максимальный срок "
                "бронирования - 1 год от текущей даты)")
            .arg(maxDate.toString("dd.MM.yyyy")));
  }

  if (normalized.endClamped) {
    QMessageBox::warning(
        this, "Предупреждение",
        QString("Дата окончания не может быть позже %1 (максимальный срок "
                "бронирования - 1 год от текущей даты)")
            .arg(maxDate.toString("dd.MM.yyyy")));
  }

  if (normalized.endAdjusted) {
    QMessageBox::warning(this, "Ошибка",
                         "Дата окончания должна быть позже даты начала");
  }

  startDateEdit_->setDate(normalized.start);
  endDateEdit_->setDate(normalized.end);

  const int days = calculator_.rentalDays(normalized.start, normalized.end);
  totalPriceLabel_->setText(
      calculator_.formattedTotal(*car_, days, currentCurrency_));
}

void RentalDialog::onRent() {
  QDate start = startDateEdit_->date();
  QDate end = endDateEdit_->date();

  if (end <= start) {
    QMessageBox::warning(this, "Ошибка",
                         "Дата окончания должна быть позже даты начала");
    return;
  }


  int availableQty =
      Database::instance().getAvailableQuantity(car_->id, start, end);
  if (availableQty <= 0) {
    QMessageBox::warning(
        this, "Недоступно",
        "Все экземпляры этого автомобиля уже забронированы на выбранные даты");
    return;
  }

  const int days = calculator_.rentalDays(start, end);
  const double total = calculator_.totalBasePrice(*car_, days);

  int rentalId = Database::instance().createRental(car_->id, customerId_, start,
                                                   end, total);
  if (rentalId > 0) {
    QMessageBox::information(
        this, "Успешно",
        QString("Аренда оформлена! Номер заказа: %1").arg(rentalId));
    accept();
  } else {
    QMessageBox::warning(this, "Ошибка", "Не удалось оформить аренду");
  }
}
