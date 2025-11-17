#pragma once

#include <QString>

class CurrencyConverter {
public:
  enum class Currency { USD, EUR, BYN };
  static CurrencyConverter &instance() {
    static CurrencyConverter inst;
    return inst;
  }

  double fromBase(double usdAmount, Currency to) const;
  double toBase(double amount, Currency from) const;

  QString symbol(Currency currency) const;
  QString code(Currency currency) const;

  static Currency fromString(const QString &str);

private:
  CurrencyConverter();
  
  double usdToEur_ = 0.92;
  double usdToByn_ = 3.28;
};

