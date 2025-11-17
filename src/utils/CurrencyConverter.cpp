#include "CurrencyConverter.h"

CurrencyConverter::CurrencyConverter() = default;

double CurrencyConverter::fromBase(double usdAmount, Currency to) const {
  using enum Currency;
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
  using enum Currency;
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
  using enum Currency;
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
  using enum Currency;
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
  using enum Currency;
  if (str == "USD")
    return USD;
  if (str == "EUR")
    return EUR;
  if (str == "BYN")
    return BYN;
  return USD;
}
