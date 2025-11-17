#include "AppException.h"

AppException::AppException(const QString &message) : message_(message) {}

const char *AppException::what() const noexcept {
  utf8Message_ = message_.toUtf8();
  return utf8Message_.constData();
}

QString AppException::getMessage() const { return message_; }


