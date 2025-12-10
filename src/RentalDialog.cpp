#include "RentalDialog.h"
#include "CustomCalendarWidget.h"
#include "db/Database.h"
#include "exceptions/DatabaseException.h"

#include <QDate>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>

RentalDialog::RentalDialog(const CarInfo &car, int customerId,
                           const QString &currency, QWidget *parent)
    : QDialog(parent), car_(new CarInfo(car)), customerId_(customerId),
      currentCurrency_(currency) {
  setWindowTitle("Оформление аренды");
  setModal(true);
  resize(350, 300);

  auto *layout = new QVBoxLayout(this);

  setupCarHeader(layout);
  setupCarSpecs(layout);
  setupDateForm(layout);
  setupPriceLabel(layout);
  setupConnections();
  setupButtons(layout);
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

  if (int availableQuantity =
          Database::instance().getAvailableQuantity(car_->id, start, end);
      availableQuantity <= 0) {
    QMessageBox::warning(
        this, "Недоступно",
        "Все экземпляры этого автомобиля уже забронированы на выбранные даты");
    return;
  }

  const int days = calculator_.rentalDays(start, end);
  const double total = calculator_.totalBasePrice(*car_, days);

  try {
    int rentalId = Database::instance().createRental(car_->id, customerId_,
                                                     start, end, total);
    QMessageBox::information(
        this, "Успешно",
        QString("Аренда оформлена! Номер заказа: %1").arg(rentalId));
    accept();
  } catch (const DatabaseException &e) {
    QMessageBox::critical(
        this, "Ошибка базы данных",
        QString("Не удалось оформить аренду:\n%1").arg(e.getMessage()));
  }
}

void RentalDialog::setupCarHeader(QVBoxLayout *layout) {
  auto *carLabel = new QLabel(
      QString("%1 %2 (%3)")
          .arg(car_->brand, car_->model, QString::number(car_->year)),
      this);
  carLabel->setStyleSheet(
      "font-size: 18px; font-weight: bold; margin-bottom: 15px;");
  layout->addWidget(carLabel);
}

void RentalDialog::setupCarSpecs(QVBoxLayout *layout) {
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
    specsLabel_->setStyleSheet("color:#ffffff; margin-bottom: 15px;");
    layout->addWidget(specsLabel_);
  }
}

void RentalDialog::setupDateForm(QVBoxLayout *layout) {
  auto *formLayout = new QFormLayout();

  QDate maxDate = QDate::currentDate().addYears(1);

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
}

void RentalDialog::setupPriceLabel(QVBoxLayout *layout) {
  totalPriceLabel_ = new QLabel("Итого: 0", this);
  totalPriceLabel_->setStyleSheet(
      "font-size: 17px; font-weight: bold; color: #111c12f; margin: 6 6 6 0;");
  layout->addWidget(totalPriceLabel_);
}

void RentalDialog::setupButtons(QVBoxLayout *layout) {
  auto *btnLayout = new QHBoxLayout();
  auto *cancelBtn = new QPushButton("Отмена", this);
  auto *rentBtn = new QPushButton("Оформить аренду", this);
  cancelBtn->setStyleSheet(
      "QPushButton { background-color: #105bee; color: "
      "white; font-weight: bold; padding: 6px; border-radius: 3px; }");
  rentBtn->setStyleSheet(
      "QPushButton { background-color: #37c62f; color: "
      "white; font-weight: bold; padding: 6px; border-radius: 3px; }");

  btnLayout->addWidget(cancelBtn);
  btnLayout->addWidget(rentBtn);
  layout->addLayout(btnLayout);

  connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
  connect(rentBtn, &QPushButton::clicked, this, &RentalDialog::onRent);
}

void RentalDialog::setupConnections() {
  connect(startDateEdit_, &QDateEdit::dateChanged, this,
          &RentalDialog::updatePrice);
  connect(endDateEdit_, &QDateEdit::dateChanged, this,
          &RentalDialog::updatePrice);
  updatePrice();
}
