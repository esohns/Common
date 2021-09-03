#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow
 : public QMainWindow
{
    Q_OBJECT

  public:
    explicit MainWindow(QWidget *parent = nullptr);
    virtual ~MainWindow();

  private slots:
    virtual void on_pushButton_2_clicked();

    virtual void on_pushButton_4_clicked();

    virtual void on_pushButton_3_clicked();

    virtual void on_pushButton_clicked();

    virtual void on_pushButton_6_clicked();

    virtual void on_pushButton_5_clicked();

  private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
