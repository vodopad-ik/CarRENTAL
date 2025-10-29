#pragma once

#include <QDialog>

class QLineEdit;
class QPushButton;

class LoginDialog : public QDialog {
  Q_OBJECT

public:
  explicit LoginDialog(QWidget *parent = nullptr);
  ~LoginDialog() override;

  QString customerName() const { return name_; }
  QString customerPhone() const { return phone_; }
  QString customerEmail() const { return email_; }
  int customerId() const { return customerId_; }

private slots:
  void onLogin();
  void onRegister();

private:
  QLineEdit *nameEdit_;
  QLineEdit *phoneEdit_;
  QLineEdit *emailEdit_;
  QPushButton *loginBtn_;
  QPushButton *registerBtn_;

  QString name_;
  QString phone_;
  QString email_;
  int customerId_ = -1;
};
