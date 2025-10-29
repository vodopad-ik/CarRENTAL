#include "CurrencyConverter.h"

CurrencyConverter &CurrencyConverter::instance() {
  static CurrencyConverter inst;
  return inst;
}

CurrencyConverter::CurrencyConverter() {
  // Set exchange rates (example rates, in real app would fetch from API)
  usdToEur_ = 0.92; // 1 USD = 0.92 EUR
  usdToByn_ = 3.28; // 1 USD = 3.28 BYN
}

double CurrencyConverter::convert(double amount, Currency from,
                                  Currency to) const {
  if (from == to)
    return amount;

  // Convert to base (USD) first
  double usdAmount = toBase(amount, from);
  // Then convert to target
  return fromBase(usdAmount, to);
}

double CurrencyConverter::fromBase(double usdAmount, Currency to) const {
  switch (to) {
  case USD:
    return usdAmount;
  case EUR:
    return usdAmount * usdToEur_;
  case BYN:
    return usdAmount * usdToByn_;
  }
  return usdAmount;
}

double CurrencyConverter::toBase(double amount, Currency from) const {
  switch (from) {
  case USD:
    return amount;
  case EUR:
    return amount / usdToEur_;
  case BYN:
    return amount / usdToByn_;
  }
  return amount;
}

QString CurrencyConverter::symbol(Currency currency) const {
  switch (currency) {
  case USD:
    return QString("$");
  case EUR:
    return QString("€");
  case BYN:
    return QString("Br");
  }
  return QString("$");
}

QString CurrencyConverter::code(Currency currency) const {
  switch (currency) {
  case USD:
    return QString("USD");
  case EUR:
    return QString("EUR");
  case BYN:
    return QString("BYN");
  }
  return QString("USD");
}

CurrencyConverter::Currency CurrencyConverter::fromString(const QString &str) {
  if (str == "USD")
    return USD;
  if (str == "EUR")
    return EUR;
  if (str == "BYN")
    return BYN;
  return USD;
}
