#include "SessionManager.h"

#include "PathsConfig.h"

#include <QDir>
#include <QFile>
#include <QIODevice>
#include <QJsonDocument>
#include <QJsonObject>
#include <QString>

QString SessionManager::sessionFilePath() const {
  return PathsConfig::sessionFile();
}

bool SessionManager::loadSession(SessionData &data) const {
  QFile sessionFile(sessionFilePath());
  if (!sessionFile.exists() || !sessionFile.open(QIODevice::ReadOnly)) {
    return false;
  }

  auto doc = QJsonDocument::fromJson(sessionFile.readAll());
  sessionFile.close();

  int sid = doc.object().value("customerId").toInt(-1);
  QString sname = doc.object().value("name").toString();

  if (sid > 0) {
    data.customerId = sid;
    data.name = sname;
    return true;
  }

  return false;
}

void SessionManager::saveSession(int customerId, const QString &name) const {
  QJsonObject obj;
  obj["customerId"] = customerId;
  obj["name"] = name;

  QFile sessionFile(sessionFilePath());
  if (sessionFile.open(QIODevice::WriteOnly)) {
    QJsonDocument doc(obj);
    sessionFile.write(doc.toJson());
    sessionFile.close();
  }
}

void SessionManager::clearSession() const { QFile::remove(sessionFilePath()); }
