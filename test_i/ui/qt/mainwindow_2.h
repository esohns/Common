#ifndef MAINWINDOW_2_H
#define MAINWINDOW_2_H

#include <QWidget>

#include "common_iinitialize.h"

template <typename MainWindowClassType,
          typename ApplicationInterfaceType> // implements Common_UI_Qt_IApplication_T
class MainWindow_T
 : public MainWindowClassType
 , public Common_IInitializeP_T<ApplicationInterfaceType>
{
  typedef MainWindowClassType inherited;

  public:
    explicit MainWindow_T(QWidget *parent = nullptr);
    inline virtual ~MainWindow_T() {}

    inline virtual bool initialize (const ApplicationInterfaceType* application_in) { application_ = const_cast<ApplicationInterfaceType*> (application_in); return true; }

  private:
    virtual void on_pushButton_2_clicked();

    virtual void on_pushButton_4_clicked();

    virtual void on_pushButton_3_clicked();

    virtual void on_pushButton_clicked();

    virtual void on_pushButton_6_clicked();

    virtual void on_pushButton_5_clicked();

    ApplicationInterfaceType* application_;
};

#include "mainwindow_2.inl"

#endif // MAINWINDOW_H
