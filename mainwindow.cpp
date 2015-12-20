#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "scene.h"

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);
  connect(ui->action,SIGNAL(toggled(bool)),this,SLOT(toPaintModeDM()));         // установка режима рисования
  connect(ui->action_2,SIGNAL(toggled(bool)),this,SLOT(toPaintModeKarta()));    // установка режима рисования
  connect(this,SIGNAL(setPaintMode(int)),ui->SceneWidget,SLOT(setPaintMode(int)));  // установка режима рисования в сцену
  connect(ui->SceneWidget->currentLevel,SIGNAL(setCameraInfo(QString)),ui->label,SLOT(setText(QString)));    // отображение информации со сцены
  connect(ui->SceneWidget,SIGNAL(setFiguresInfo(QString)),ui->label_2,SLOT(setText(QString)));      // отображение информации со сцены
  connect(ui->SceneWidget,SIGNAL(setFiguresInfo2(QString)),ui->label_4,SLOT(setText(QString)));     // отображение информации со сцены
  connect(ui->SceneWidget,SIGNAL(setBar(QString)),ui->statusBar,SLOT(showMessage(QString)));     // отображение индикаторов состояния
  connect(ui->comboBox,SIGNAL(currentIndexChanged(int)),ui->SceneWidget,SLOT(setPerspective(int))); // переключение вида проекции
  connect(ui->action_3,SIGNAL(triggered(bool)),ui->SceneWidget,SLOT(buildNewMap()));  // открытие файла и построение новой карты
  connect(ui->action_4,SIGNAL(triggered(bool)),ui->SceneWidget,SLOT(addMap()));  // открытие файла и добавление новых данных к карте
  //ui->comboBox->setWindowOpacity(0.1);
  AG=new QActionGroup(this);    // оформление тригерного меню
  AG->addAction(ui->action);
  AG->addAction(ui->action_2);
  ui->action->setChecked(true);

  //ui->lineEdit->setStyleSheet("QLineEdit {background-color: rgba(0, 0, 0, 0);}");
  ui->comboBox->setStyleSheet("QComboBox {background-color: rgba(0, 0, 0, 0);}");
}

MainWindow::~MainWindow()
{
  delete ui;
}

void MainWindow::toPaintModeDM()    // установка режима рисования
{
  ui->comboBox->setVisible(true);
  ui->label->setVisible(true);
  ui->label_2->setVisible(true);
  ui->label_3->setVisible(true);
  ui->label_4->setVisible(true);
  ui->label_5->setVisible(true);
  emit setPaintMode(1);             //TEST (DEVELOPER_MODE)
}

void MainWindow::toPaintModeKarta() // установка режима рисования
{
  ui->comboBox->setVisible(false);
  ui->label->setVisible(false);
  ui->label_2->setVisible(false);
  ui->label_3->setVisible(false);
  ui->label_4->setVisible(false);
  ui->label_5->setVisible(false);
  ui->comboBox->setCurrentIndex(0);
  emit setPaintMode(2);             // KARTA MODE
}
