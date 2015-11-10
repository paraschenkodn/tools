#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QActionGroup>

namespace Ui {
  class MainWindow;
}

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();

private:
  Ui::MainWindow *ui;
  QActionGroup *AG;

signals:
  // устанавливаем режим рисования
  void setPaintMode(int);

public slots:
  // устанавливаем режим рисования
  void toPaintModeDM();
  void toPaintModeKarta();
private slots:
};

#endif // MAINWINDOW_H
