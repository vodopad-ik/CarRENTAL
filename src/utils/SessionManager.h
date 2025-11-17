#pragma once

#include <QString>

class SessionManager {
public:
  static inline SessionManager &instance() {
    static SessionManager inst;
    return inst;
  }

  struct SessionData {
    int customerId = -1;
    QString name;
  };

  bool loadSession(SessionData &data) const;
  void saveSession(int customerId, const QString &name) const;
  void clearSession() const;

private:
  SessionManager() = default;
  
  QString sessionFilePath() const;
};













