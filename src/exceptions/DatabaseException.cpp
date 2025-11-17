#include "DatabaseException.h"

DatabaseException::DatabaseException(const QString &operation,
                                     const QString &details)
    : AppException(QString("Database error during '%1': %2")
                       .arg(operation, details)),
      operation_(operation) {}





