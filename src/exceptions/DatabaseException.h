#pragma once

#include "AppException.h"

class DatabaseException : public AppException {
public:
  explicit DatabaseException(const QString &operation, const QString &details);
  QString getOperation() const { return operation_; }

private:
  QString operation_;
};




