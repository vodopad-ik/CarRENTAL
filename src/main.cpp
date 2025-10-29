#include "MainWindow.h"
#include <QApplication>
#include <QCursor>
#include <QStyle>

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  QApplication::setApplicationName("CarRental");
  QApplication::setOrganizationName("CarRental");

  // Устанавливаем стандартный курсор для всего приложения
  app.setOverrideCursor(QCursor(Qt::ArrowCursor));
  app.restoreOverrideCursor();

  MainWindow w;
  w.show();
  return app.exec();
}
