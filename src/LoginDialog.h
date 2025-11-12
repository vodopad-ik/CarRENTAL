#pragma once

#include <QDialog>

class QLineEdit;
class QPushButton;
class QLabel;

class LoginDialog : public QDialog {
  Q_OBJECT

public:
  explicit LoginDialog(QWidget *parent = nullptr);
  ~LoginDialog() override;

  QString customerName() const { return name_; }
  int customerId() const { return customerId_; }

private slots:
  void onLogin();
  void onRegister();

private:
  QLineEdit *nameEdit_;
  QLineEdit *phoneEdit_;
  QLineEdit *passwordEdit_;
  QLineEdit *passwordRepeatEdit_;
  QLabel *nameLabel_ = nullptr;
  QLabel *passwordRepeatLabel_ = nullptr;
  QPushButton *loginBtn_;
  QPushButton *registerBtn_;

  QString name_;
  QString phone_;
  int customerId_ = -1;
};
