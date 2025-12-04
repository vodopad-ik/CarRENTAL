#pragma once

#include <QDialog>

class QLineEdit;
class QPushButton;
class QLabel;
class QVBoxLayout;

class LoginDialog : public QDialog {
  Q_OBJECT

public:
  explicit LoginDialog(QWidget *parent = nullptr);
  ~LoginDialog() override = default;

  QString getCustomerName() const { return name_; }
  int getCustomerId() const { return customerId_; }

private slots:
  void onLogin();
  void onRegister();

private:
  void setupHeader(QVBoxLayout *layout);
  void setupTabs(QVBoxLayout *layout);
  void setupForm(QVBoxLayout *layout);
  void setupActionButtons(QVBoxLayout *layout);
  void setupConnections();

  QLineEdit *nameEdit_;
  QLineEdit *phoneEdit_;
  QLineEdit *passwordEdit_;
  QLineEdit *passwordRepeatEdit_;
  QLabel *nameLabel_ = nullptr;
  QLabel *passwordRepeatLabel_ = nullptr;
  QPushButton *loginBtn_;
  QPushButton *registerBtn_;
  QPushButton *tabLogin_ = nullptr;
  QPushButton *tabRegister_ = nullptr;

  QString name_;
  QString phone_;
  int customerId_ = -1;
};
