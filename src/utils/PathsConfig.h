#pragma once

#include <QString>

class PathsConfig {
public:
  static QString dataDir();
  static QString databaseFile();
  static QString sessionFile();
  static QString photosDir();
  static QString placeholderImage();
};

