#include "SessionManager.h"

#include "PathsConfig.h"

#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>

SessionManager &SessionManager::instance() {
  static SessionManager inst;
  return inst;
}

QString SessionManager::sessionFilePath() const {
  return PathsConfig::sessionFile();
}

bool SessionManager::loadSession(SessionData &data) {
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

void SessionManager::saveSession(int customerId, const QString &name) {
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

void SessionManager::clearSession() {
  QFile::remove(sessionFilePath());
}




