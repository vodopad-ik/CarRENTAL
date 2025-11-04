#include "MainWindow.h"
#include <QApplication>
#include <QStyle>

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  QApplication::setApplicationName("CarRental");
  QApplication::setOrganizationName("CarRental");

  MainWindow w;
  // Не показываем окно сразу - оно покажется после успешного входа
  // или скроется, если пользователь закроет диалог входа
  return app.exec();
}
