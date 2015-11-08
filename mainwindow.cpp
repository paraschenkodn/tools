#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "scene.h"

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);
  connect(ui->action,SIGNAL(toggled(bool)),this,SLOT(toPaintModeDM()));
  connect(ui->action_2,SIGNAL(toggled(bool)),this,SLOT(toPaintModeKarta()));
  connect(this,SIGNAL(setPaintMode(int)),ui->SceneWidget,SLOT(setPaintMode(int)));
  connect(ui->SceneWidget,SIGNAL(setPerspectiveInfo(QString)),ui->label,SLOT(setText(QString)));
  connect(ui->SceneWidget,SIGNAL(setFiguresInfo(QString)),ui->label_2,SLOT(setText(QString)));
  connect(ui->SceneWidget,SIGNAL(setFiguresInfo2(QString)),ui->label_4,SLOT(setText(QString)));
  connect(ui->comboBox,SIGNAL(currentIndexChanged(int)),ui->SceneWidget,SLOT(setPerspective(int)));
  //ui->comboBox->setWindowOpacity(0.1);
  AG=new QActionGroup(this);
  AG->addAction(ui->action);
  AG->addAction(ui->action_2);
  ui->action->setChecked(true);
}

MainWindow::~MainWindow()
{
  delete ui;
}

void MainWindow::toPaintModeDM()
{
  ui->comboBox->setVisible(true);
  ui->label->setVisible(true);
  ui->label_2->setVisible(true);
  ui->label_3->setVisible(true);
  ui->label_4->setVisible(true);
  emit setPaintMode(1);
}

void MainWindow::toPaintModeKarta()
{
  ui->comboBox->setVisible(false);
  ui->label->setVisible(false);
  ui->label_2->setVisible(false);
  ui->label_3->setVisible(false);
  ui->label_4->setVisible(false);
  ui->comboBox->setCurrentIndex(0);
  emit setPaintMode(2);
}
