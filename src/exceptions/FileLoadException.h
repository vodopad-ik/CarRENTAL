#pragma once

#include "AppException.h"

class FileLoadException : public AppException {
public:
  explicit FileLoadException(const QString &filePath, const QString &reason);
  QString getFilePath() const { return filePath_; }

private:
  QString filePath_;
};




