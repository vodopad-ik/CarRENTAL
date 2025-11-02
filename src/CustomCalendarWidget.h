#pragma once

#include <QCalendarWidget>
#include <QDate>

class CustomCalendarWidget : public QCalendarWidget {
  Q_OBJECT

public:
  explicit CustomCalendarWidget(int carId, QWidget *parent = nullptr);
  void setCarId(int carId);

protected:
  void showEvent(QShowEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;

private:
  int carId_;
  bool isDateBooked(const QDate &date) const;
};
