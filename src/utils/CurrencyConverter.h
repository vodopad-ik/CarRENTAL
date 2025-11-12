#pragma once

#include <QString>

class CurrencyConverter {
public:
  enum Currency { USD, EUR, BYN };
  static CurrencyConverter &instance();


  double fromBase(double usdAmount, Currency to) const;


  double toBase(double amount, Currency from) const;


  QString symbol(Currency currency) const;


  QString code(Currency currency) const;


  static Currency fromString(const QString &str);

private:
  CurrencyConverter();


  double usdToEur_;
  double usdToByn_;
};

