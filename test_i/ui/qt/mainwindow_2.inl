#include <iostream>

#include <QApplication>
#include <QPushButton>

#include "ace/Log_Msg.h"

template <typename MainWindowClassType,
          typename ApplicationInterfaceType>
MainWindow_T<MainWindowClassType,
             ApplicationInterfaceType>::MainWindow_T (QWidget *parent)
 : MainWindowClassType(parent)
 , application_ (NULL)
{

}

template <typename MainWindowClassType,
          typename ApplicationInterfaceType>
void
MainWindow_T<MainWindowClassType,
             ApplicationInterfaceType>::on_pushButton_2_clicked()
{
  QPushButton* button_p =
//      Q_CHILD( this, QPushButton, "pushButton_2" );
      QObject::findChild<QPushButton*> ("pushButton_2");
  ACE_ASSERT(button_p);
  if (button_p->isChecked())
    std::cout << "checked" << std::endl;
  else
    std::cout << "unchecked" << std::endl;
}

template <typename MainWindowClassType,
          typename ApplicationInterfaceType>
void
MainWindow_T<MainWindowClassType,
             ApplicationInterfaceType>::on_pushButton_4_clicked()
{
  std::cout << "testsss 2" << std::endl;
}

template <typename MainWindowClassType,
          typename ApplicationInterfaceType>
void
MainWindow_T<MainWindowClassType,
             ApplicationInterfaceType>::on_pushButton_3_clicked()
{
  std::cout << "testsss 3" << std::endl;
}

template <typename MainWindowClassType,
          typename ApplicationInterfaceType>
void
MainWindow_T<MainWindowClassType,
             ApplicationInterfaceType>::on_pushButton_clicked()
{
  std::cout << "testsss 4" << std::endl;
}

template <typename MainWindowClassType,
          typename ApplicationInterfaceType>
void
MainWindow_T<MainWindowClassType,
             ApplicationInterfaceType>::on_pushButton_6_clicked()
{
  std::cout << "testsss 5" << std::endl;
}

template <typename MainWindowClassType,
          typename ApplicationInterfaceType>
void
MainWindow_T<MainWindowClassType,
             ApplicationInterfaceType>::on_pushButton_5_clicked()
{
  QApplication::quit();
}
