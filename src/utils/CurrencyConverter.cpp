#include "CurrencyConverter.h"

CurrencyConverter::CurrencyConverter() = default;

double CurrencyConverter::fromBase(double usdAmount, Currency to) const {
  switch (to) {
  case Currency::USD:
    return usdAmount;
  case Currency::EUR:
    return usdAmount * usdToEur_;
  case Currency::BYN:
    return usdAmount * usdToByn_;
  }
  return usdAmount;
}

double CurrencyConverter::toBase(double amount, Currency from) const {
  switch (from) {
  case Currency::USD:
    return amount;
  case Currency::EUR:
    return amount / usdToEur_;
  case Currency::BYN:
    return amount / usdToByn_;
  }
  return amount;
}

QString CurrencyConverter::symbol(Currency currency) const {
  switch (currency) {
  case Currency::USD:
    return QString("$");
  case Currency::EUR:
    return QString("€");
  case Currency::BYN:
    return QString("Br");
  }
  return QString("$");
}

QString CurrencyConverter::code(Currency currency) const {
  switch (currency) {
  case Currency::USD:
    return QString("USD");
  case Currency::EUR:
    return QString("EUR");
  case Currency::BYN:
    return QString("BYN");
  }
  return QString("USD");
}

CurrencyConverter::Currency CurrencyConverter::fromString(const QString &str) {
  if (str == "USD")
    return Currency::USD;
  if (str == "EUR")
    return Currency::EUR;
  if (str == "BYN")
    return Currency::BYN;
  return Currency::USD;
}
