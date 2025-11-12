#pragma once

#include <QPixmap>
#include <QSize>
#include <QString>

class CarImageLoader {
public:
  static QPixmap loadCardImage(const QString &resourcePath,
                               const QSize &targetSize);

private:
  static QString resolveKey(const QString &resourcePath);
  static QPixmap roundCorners(const QPixmap &source, int radius);
};

