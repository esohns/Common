#include "stdafx.h"

#include <QApplication>

#include "mainwindow.h"

#undef main
int main (int argc, char *argv[])
{
  QApplication a (argc, argv);
  MainWindow w;
  w.show ();

  return a.exec ();
}
