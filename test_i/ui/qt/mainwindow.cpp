#include "stdafx.h"

#include "mainwindow.h"

#include <iostream>

#include "ui_mainwindow.h"

MainWindow::MainWindow (QWidget *parent)
 : QMainWindow (parent),
   ui (new Ui::MainWindow)
{
  ui->setupUi (this);
}

MainWindow::~MainWindow ()
{
  delete ui;
}

void MainWindow::on_pushButton_2_clicked ()
{
//  QPushButton* button_p = this->findChild<QPushButton*>("pushButton_2");
//  Q_ASSERT(button_p);
//  if (button_p->isChecked())
//    std::cout << "checked" << std::endl;
//  else
//    std::cout << "unchecked" << std::endl;
}

void MainWindow::on_pushButton_4_clicked ()
{
//  std::cout << "testsss 2" << std::endl;
}

void MainWindow::on_pushButton_3_clicked ()
{
//  std::cout << "testsss 3" << std::endl;
}

void MainWindow::on_pushButton_clicked ()
{
//  std::cout << "testsss 4" << std::endl;
}

void MainWindow::on_pushButton_6_clicked ()
{
//  std::cout << "testsss 5" << std::endl;
}

void MainWindow::on_pushButton_5_clicked ()
{
//  QApplication::quit();
}
