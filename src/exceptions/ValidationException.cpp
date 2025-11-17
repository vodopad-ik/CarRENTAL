#include "ValidationException.h"

ValidationException::ValidationException(const QString &field,
                                         const QString &reason)
    : AppException(QString("Validation error for field '%1': %2")
                       .arg(field, reason)),
      field_(field) {}




