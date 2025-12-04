#include "LoginDialog.h"
#include "db/Database.h"

#include <QCryptographicHash>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QRegularExpression>
#include <QSqlQuery>
#include <QStyle>
#include <QVBoxLayout>

LoginDialog::LoginDialog(QWidget *parent) : QDialog(parent) {
  setWindowTitle("Вход / Регистрация");
  setModal(true);
  resize(400, 300);

  auto *layout = new QVBoxLayout(this);

  setupHeader(layout);
  setupTabs(layout);
  setupForm(layout);
  setupActionButtons(layout);
  setupConnections();
}

void LoginDialog::onLogin() {
  QString phone = phoneEdit_->text().trimmed();
  QString password = passwordEdit_->text();
  if (phone.isEmpty()) {
    QMessageBox::warning(this, "Ошибка", "Введите телефон");
    return;
  }

  if (QRegularExpression re("^\\+375\\d{9}$"); !re.match(phone).hasMatch()) {
    QMessageBox::warning(this, "Ошибка",
                         "Телефон должен быть в формате +375xxxxxxxxx");
    return;
  }
  if (password.isEmpty()) {
    QMessageBox::warning(this, "Ошибка", "Введите пароль");
    return;
  }

  QByteArray passHash =
      QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256)
          .toHex();
  int id = Database::instance().findCustomerByPhoneAndPassword(
      phone, QString::fromUtf8(passHash));
  if (id > 0) {
    customerId_ = id;

    QSqlDatabase dbConn = QSqlDatabase::database("carrental");
    QSqlQuery q(dbConn);
    q.prepare("SELECT name FROM customers WHERE id = ?");
    q.addBindValue(id);
    if (q.exec() && q.next())
      name_ = q.value(0).toString();
    phone_ = phone;
    accept();
  } else {
    QMessageBox::information(this, "Не найден",
                             "Неверный телефон или пароль. Если вы не "
                             "зарегистрированы, выполните регистрацию.");
  }
}

void LoginDialog::onRegister() {
  name_ = nameEdit_->text().trimmed();
  phone_ = phoneEdit_->text().trimmed();
  QString password = passwordEdit_->text();
  QString password2 = passwordRepeatEdit_->text();

  if (name_.isEmpty()) {
    QMessageBox::warning(this, "Ошибка", "Введите имя");
    return;
  }
  if (QRegularExpression re("^\\+375\\d{9}$"); !re.match(phone_).hasMatch()) {
    QMessageBox::warning(this, "Ошибка",
                         "Телефон должен быть в формате +375xxxxxxxxx");
    return;
  }

  {
    QSqlDatabase dbConn = QSqlDatabase::database("carrental");
    QSqlQuery q(dbConn);
    q.prepare("SELECT COUNT(*) FROM customers WHERE phone = ?");
    q.addBindValue(phone_);
    if (q.exec() && q.next() && q.value(0).toInt() > 0) {
      QMessageBox::warning(
          this, "Аккаунт существует",
          "Пользователь с таким номером телефона уже зарегистрирован!");
      return;
    }
  }

  if (password.size() < 6) {
    QMessageBox::warning(this, "Ошибка",
                         "Пароль должен содержать минимум 6 символов");
    return;
  }
  if (password != password2) {
    QMessageBox::warning(this, "Ошибка", "Пароли не совпадают");
    return;
  }

  QByteArray passHash =
      QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256)
          .toHex();
  customerId_ = Database::instance().addCustomer(name_, phone_,
                                                 QString::fromUtf8(passHash));
  if (customerId_ > 0) {
    accept();
  } else {
    QMessageBox::warning(this, "Ошибка", "Не удалось зарегистрировать клиента");
  }
}

void LoginDialog::setupHeader(QVBoxLayout *layout) {
  auto *title = new QLabel("Аренда автомобилей", this);
  title->setStyleSheet(
      "font-size: 20px; font-weight: bold; margin-bottom: 20px;");
  title->setAlignment(Qt::AlignCenter);
  layout->addWidget(title);

  auto *welcome = new QLabel(
      "Доброго времени суток, уважаемый пользователь!\n\nВыберите действие:",
      this);
  welcome->setStyleSheet(
      "font-size: 16px; font-weight: normal; margin-bottom: 10px;");
  welcome->setAlignment(Qt::AlignCenter);
  layout->addWidget(welcome);
}

void LoginDialog::setupTabs(QVBoxLayout *layout) {
  auto *tabsLayout = new QHBoxLayout();
  tabLogin_ = new QPushButton("Войти", this);
  tabRegister_ = new QPushButton("Зарегистрироваться", this);
  tabLogin_->setCheckable(true);
  tabRegister_->setCheckable(true);
  tabLogin_->setChecked(true);

  const QString tabBtnStyle =
      "QPushButton { font-size: 15px; padding: 10px 24px; border: 1px solid "
      "#555; border-radius: 6px; }"
      "QPushButton:hover { background: #3a3a3a; }"
      "QPushButton:checked { background:rgb(0, 46, 115); color: white; "
      "border-color: "
      "#2d7ef7; }";
  tabLogin_->setStyleSheet(tabBtnStyle);
  tabRegister_->setStyleSheet(tabBtnStyle);
  tabLogin_->setIcon(style()->standardIcon(QStyle::SP_DialogOkButton));
  tabRegister_->setIcon(style()->standardIcon(QStyle::SP_DialogApplyButton));
  tabLogin_->setMinimumHeight(40);
  tabRegister_->setMinimumHeight(40);
  tabsLayout->addWidget(tabLogin_);
  tabsLayout->addWidget(tabRegister_);
  layout->addLayout(tabsLayout);
}

void LoginDialog::setupForm(QVBoxLayout *layout) {
  auto *formLayout = new QFormLayout();
  phoneEdit_ = new QLineEdit(this);
  phoneEdit_->setPlaceholderText("+375xxxxxxxxx");
  passwordEdit_ = new QLineEdit(this);
  passwordEdit_->setEchoMode(QLineEdit::Password);
  nameEdit_ = new QLineEdit(this);
  passwordRepeatEdit_ = new QLineEdit(this);
  passwordRepeatEdit_->setEchoMode(QLineEdit::Password);

  nameLabel_ = new QLabel("Имя:", this);
  passwordRepeatLabel_ = new QLabel("Повторите пароль:", this);

  formLayout->addRow(nameLabel_, nameEdit_);
  formLayout->addRow("Телефон:", phoneEdit_);
  formLayout->addRow("Пароль:", passwordEdit_);
  formLayout->addRow(passwordRepeatLabel_, passwordRepeatEdit_);
  layout->addLayout(formLayout);
}

void LoginDialog::setupActionButtons(QVBoxLayout *layout) {
  loginBtn_ = new QPushButton("Войти", this);
  registerBtn_ = new QPushButton("Зарегистрироваться", this);
  loginBtn_->setIcon(style()->standardIcon(QStyle::SP_DialogOkButton));
  registerBtn_->setIcon(style()->standardIcon(QStyle::SP_DialogApplyButton));

  const QString actionBtnStyle = "QPushButton { padding: 12px 18px; font-size: "
                                 "15px; border-radius: 8px; }";
  loginBtn_->setStyleSheet(
      actionBtnStyle +
      "QPushButton { background:#2d7ef7; color:white; }"
      "QPushButton:hover { background-color:rgb(0, 46, 115); }");
  registerBtn_->setStyleSheet(
      actionBtnStyle + "QPushButton { background:#4caf50; color:white; }"
                       "QPushButton:hover { background-color: #4fcf50; }");
  loginBtn_->setMinimumHeight(44);
  registerBtn_->setMinimumHeight(44);
  layout->addWidget(loginBtn_);
  layout->addWidget(registerBtn_);

  nameLabel_->setVisible(false);
  nameEdit_->setVisible(false);
  passwordRepeatLabel_->setVisible(false);
  passwordRepeatEdit_->setVisible(false);
  registerBtn_->setVisible(false);
}

void LoginDialog::setupConnections() {
  auto setLoginMode = [this]() {
    tabLogin_->setChecked(true);
    tabRegister_->setChecked(false);
    nameLabel_->setVisible(false);
    nameEdit_->setVisible(false);
    passwordRepeatLabel_->setVisible(false);
    passwordRepeatEdit_->setVisible(false);
    loginBtn_->setVisible(true);
    registerBtn_->setVisible(false);
  };
  auto setRegisterMode = [this]() {
    tabLogin_->setChecked(false);
    tabRegister_->setChecked(true);
    nameLabel_->setVisible(true);
    nameEdit_->setVisible(true);
    passwordRepeatLabel_->setVisible(true);
    passwordRepeatEdit_->setVisible(true);
    loginBtn_->setVisible(false);
    registerBtn_->setVisible(true);
  };
  connect(tabLogin_, &QPushButton::clicked, setLoginMode);
  connect(tabRegister_, &QPushButton::clicked, setRegisterMode);

  connect(loginBtn_, &QPushButton::clicked, this, &LoginDialog::onLogin);
  connect(registerBtn_, &QPushButton::clicked, this, &LoginDialog::onRegister);
}
