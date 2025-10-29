#include "LoginDialog.h"
#include "db/Database.h"

#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QSqlQuery>
#include <QVBoxLayout>

LoginDialog::LoginDialog(QWidget *parent) : QDialog(parent), customerId_(-1) {
  setWindowTitle("Вход / Регистрация");
  setModal(true);
  resize(400, 300);

  auto *layout = new QVBoxLayout(this);

  auto *title = new QLabel("Аренда автомобилей", this);
  title->setStyleSheet(
      "font-size: 20px; font-weight: bold; margin-bottom: 20px;");
  title->setAlignment(Qt::AlignCenter);
  layout->addWidget(title);

  auto *formLayout = new QFormLayout();

  nameEdit_ = new QLineEdit(this);
  nameEdit_->setPlaceholderText("Иван Иванов");
  formLayout->addRow("Имя:", nameEdit_);

  phoneEdit_ = new QLineEdit(this);
  phoneEdit_->setPlaceholderText("+7 (999) 123-45-67");
  formLayout->addRow("Телефон:", phoneEdit_);

  emailEdit_ = new QLineEdit(this);
  emailEdit_->setPlaceholderText("email@example.com");
  formLayout->addRow("Email:", emailEdit_);

  layout->addLayout(formLayout);

  auto *btnLayout = new QHBoxLayout();
  loginBtn_ = new QPushButton("Войти", this);
  registerBtn_ = new QPushButton("Зарегистрироваться", this);

  loginBtn_->setStyleSheet("QPushButton { padding: 10px; font-size: 14px; }");
  registerBtn_->setStyleSheet("QPushButton { padding: 10px; font-size: 14px; "
                              "background-color: #4CAF50; color: white; }");

  btnLayout->addWidget(loginBtn_);
  btnLayout->addWidget(registerBtn_);
  layout->addLayout(btnLayout);

  connect(loginBtn_, &QPushButton::clicked, this, &LoginDialog::onLogin);
  connect(registerBtn_, &QPushButton::clicked, this, &LoginDialog::onRegister);
}

LoginDialog::~LoginDialog() {}

void LoginDialog::onLogin() {
  QString phone = phoneEdit_->text().trimmed();
  if (phone.isEmpty()) {
    QMessageBox::warning(this, "Ошибка", "Введите телефон");
    return;
  }

  // Note: Database needs to expose db_ - we'll fix this
  // For now, use a workaround
  auto &db = Database::instance();
  QSqlDatabase dbConn = QSqlDatabase::database("carrental");
  QSqlQuery q(dbConn);
  q.prepare("SELECT id, name, email FROM customers WHERE phone = ?");
  q.addBindValue(phone);

  if (q.exec() && q.next()) {
    customerId_ = q.value(0).toInt();
    name_ = q.value(1).toString();
    phone_ = phone;
    email_ = q.value(2).toString();
    accept();
  } else {
    QMessageBox::information(
        this, "Не найден",
        "Клиент с таким телефоном не найден. Зарегистрируйтесь.");
  }
}

void LoginDialog::onRegister() {
  name_ = nameEdit_->text().trimmed();
  phone_ = phoneEdit_->text().trimmed();
  email_ = emailEdit_->text().trimmed();

  if (name_.isEmpty() || phone_.isEmpty()) {
    QMessageBox::warning(this, "Ошибка", "Заполните имя и телефон");
    return;
  }

  customerId_ = Database::instance().addCustomer(name_, phone_, email_);
  if (customerId_ > 0) {
    accept();
  } else {
    QMessageBox::warning(this, "Ошибка", "Не удалось зарегистрировать клиента");
  }
}
