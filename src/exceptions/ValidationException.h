#pragma once

#include "AppException.h"

class ValidationException : public AppException {
public:
  explicit ValidationException(const QString &field, const QString &reason);
  QString getField() const { return field_; }

private:
  QString field_;
};




