#include "MainWindow.h"
#include <QApplication>

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  QApplication::setApplicationName("CarRental");
  QApplication::setOrganizationName("CarRental");

  MainWindow w;

  return app.exec();
}
