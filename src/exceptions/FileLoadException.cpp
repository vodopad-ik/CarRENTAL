#include "FileLoadException.h"

FileLoadException::FileLoadException(const QString &filePath,
                                     const QString &reason)
    : AppException(QString("Failed to load file '%1': %2")
                       .arg(filePath, reason)),
      filePath_(filePath) {}





