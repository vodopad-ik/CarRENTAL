#pragma once

#include <QString>

class CurrencyConverter {
public:
  enum Currency { USD, EUR, BYN };

  static CurrencyConverter &instance();

  // Convert price from base currency (USD) to target currency
  double convert(double amount, Currency from, Currency to) const;

  // Convert from base (USD) to target
  double fromBase(double usdAmount, Currency to) const;

  // Convert to base (USD) from source
  double toBase(double amount, Currency from) const;

  // Get currency symbol
  QString symbol(Currency currency) const;

  // Get currency code
  QString code(Currency currency) const;

  // Parse currency from string
  static Currency fromString(const QString &str);

private:
  CurrencyConverter();
  
  // Exchange rates relative to USD (base currency)
  double usdToEur_;
  double usdToByn_;
};

