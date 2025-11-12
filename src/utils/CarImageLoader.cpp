#include "CarImageLoader.h"

#include "PathsConfig.h"

#include <QDir>
#include <QFile>
#include <QPainter>
#include <QPainterPath>

QPixmap CarImageLoader::loadCardImage(const QString &resourcePath,
                                      const QSize &targetSize) {
  const QString key = resolveKey(resourcePath);
  const QString photosBase = PathsConfig::photosDir();

  const QStringList extensions = {".jpg", ".jpeg", ".png"};
  QPixmap pixmap;

  for (const auto &ext : extensions) {
    const QString candidate = photosBase + "/" + key + ext;
    if (QFile::exists(candidate) && pixmap.load(candidate))
      break;
  }

  if (pixmap.isNull()) {
    pixmap.load(PathsConfig::placeholderImage());
  }

  if (pixmap.isNull())
    return pixmap;

  QPixmap scaled = pixmap.scaled(targetSize, Qt::KeepAspectRatioByExpanding,
                                 Qt::SmoothTransformation);
  const QPoint topLeft((scaled.width() - targetSize.width()) / 2,
                       (scaled.height() - targetSize.height()) / 2);
  QPixmap cropped = scaled.copy(QRect(topLeft, targetSize));
  return roundCorners(cropped, 12);
}

QString CarImageLoader::resolveKey(const QString &resourcePath) {
  if (resourcePath.isEmpty())
    return QString();

  QString key = resourcePath;
  const int start = key.lastIndexOf('/') + 1;
  const int end = key.lastIndexOf('.');
  if (start >= 0 && end > start)
    return key.mid(start, end - start);

  key = key.section('/', -1);
  return key.section('.', 0, 0);
}

QPixmap CarImageLoader::roundCorners(const QPixmap &source, int radius) {
  if (source.isNull())
    return source;

  QPixmap target(source.size());
  target.fill(Qt::transparent);

  QPainter painter(&target);
  painter.setRenderHint(QPainter::Antialiasing, true);
  QPainterPath path;
  path.addRoundedRect(source.rect(), radius, radius);
  painter.setClipPath(path);
  painter.drawPixmap(0, 0, source);
  return target;
}

