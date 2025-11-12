#pragma once

#include <QString>

class SessionManager {
public:
  static SessionManager &instance();

  struct SessionData {
    int customerId = -1;
    QString name;
  };

  bool loadSession(SessionData &data);
  void saveSession(int customerId, const QString &name);
  void clearSession();

private:
  SessionManager() = default;
  QString sessionFilePath() const;
};





