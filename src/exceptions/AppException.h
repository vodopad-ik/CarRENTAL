#pragma once

#include <QString>
#include <exception>

class AppException : public std::exception {
public:
  explicit AppException(const QString &message);
  const char *what() const noexcept override;
  QString getMessage() const;

private:
  QString message_;
  mutable QByteArray utf8Message_;
};



