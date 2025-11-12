#include "PathsConfig.h"

#include <QCoreApplication>
#include <QDir>

QString PathsConfig::dataDir() {
  const QString dir = QDir::homePath() + "/.carrental";
  QDir().mkpath(dir);
  return dir;
}

QString PathsConfig::databaseFile() { return dataDir() + "/app.db"; }

QString PathsConfig::sessionFile() { return dataDir() + "/session.json"; }

QString PathsConfig::photosDir() {
  const QString binDir = QCoreApplication::applicationDirPath();
  return QDir(binDir + "/../../resources/photos/cars/").absolutePath();
}

QString PathsConfig::placeholderImage() {
  const QString binDir = QCoreApplication::applicationDirPath();
  return QDir(binDir + "/../../resources/images/placeholder.svg").absolutePath();
}

