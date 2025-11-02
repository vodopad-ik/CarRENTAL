#include "MainWindow.h"
#include <QApplication>
#include <QCursor>
#include <QStyle>

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  QApplication::setApplicationName("CarRental");
  QApplication::setOrganizationName("CarRental");

  // Устанавливаем стандартный курсор для всего приложения (нормальный размер)
  QApplication::setOverrideCursor(QCursor(Qt::ArrowCursor));
  QApplication::restoreOverrideCursor();

  MainWindow w;
  // Не показываем окно сразу - оно покажется после успешного входа
  // или скроется, если пользователь закроет диалог входа
  return app.exec();
}
