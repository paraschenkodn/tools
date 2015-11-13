#ifndef SCENE_H
#define SCENE_H

#include <QOpenGLWidget>
#include "triangle.h"
#include "pointsofsphere.h"
#include "shphere.h"
#include "text.h"
#include "mapbuilder.h"

#include <QOpenGLShaderProgram>
#include <QKeyEvent>
#include <QTimer>
#include <QOpenGLFunctions>
#include <QOpenGLFunctions_3_0>

/// PAINT MODE
#define TEST_MODE   1
#define KARTA_MODE  2

class Scene : public QOpenGLWidget
{
  Q_OBJECT
public:
  Scene(QWidget *parent = 0);
  ~Scene();

  QVector4D viewport;

private:
  void initializeGL();
  void paintGL();
  void paintDM(); // рисование в режиме разработчика
  void paintKarta(); // рисование в режиме построения карты (InterNet-Router-SubNet-LAN-Node)
  void paintFlatMap(); // рисование плоской карты
  void paintSphereMap(); // рисование сферической карты
  void resizeGL(int w, int h);

  // определяем метод изменяющий координаты положения треугольника
  void keyPressEvent(QKeyEvent *event);
  void mousePressEvent(QMouseEvent *event);      // работка с мышью, переопределяем функции обработки сообщений мыши (нажатие кнопок)
  void mouseReleaseEvent(QMouseEvent *event);    // переопределяем функции обработки сообщений мыши (отпускание кнопки)
  ///*********
  void mouseMoveEvent(QMouseEvent *event);       // переопределяем функции обработки сообщений мыши (перемещение)
  ///* если будем водить мышью по форме диалога с нажатой клавишей, сообщения также получает сцена
  /// ********
  void wheelEvent(QWheelEvent * event);  // переопределяем функции обработки сообщений мыши (колесо)

  // переменные фигур
  float step; // шаг сдвига фигур

  // переменные камеры
  float ratio;
  QVector3D cameraEye;
  QVector3D cameraCenter;
  QVector3D cameraUp;
  float cameraFocusAngle;
  //int camEyePos; // позиция камеры () // + control of board??
  void setCameraInfo();  // формируем и посылаем текст для отображения параметров в главном окне
  // енд камера

  Triangle *m_triangle;
  QOpenGLShaderProgram m_program;
  //QVector<QOpenGLShaderProgram *> m_program;
  shphere *m_shphere;
  pointsofsphere *spherepoints;
  Text *m_text;

  MapBuilder *buildermap;   // класс картостроителя

  void setFigureInfo(); // формируем и посылаем текст для отображения параметров в главном окне

  // создаём идентификаторы для обращения к шейдерным переменным
  int m_vertexAttr;
  int m_colorAttr;
  int m_matrixUniform;
  int m_texAttr;
  int m_texUniform;

  QTimer m_timer;
  int m_angle; // текущий угол поворота

  void setLights();
  void setStates();
  void defaultStates();

  bool perspective;  // признак рисования в перспективной или в ортогональной проекции
  int paintMode; // режим рисования
public slots:
  // устанавливаем признак рисования в перспективной или ортогональной проекции
  void setPerspective(int _switch);
  // Установка режима рисования
  void setPaintMode(int mode);  // 1 - test, 2 - karta
  // строим новую карту
  void buildNewMap();
  // добавляем в карту
  void addMap();
private slots:
  // заставляем прорисовываться по таймеру
  void slotAnimation();

signals:
  void setPerspectiveInfo(const QString &);
  void setFiguresInfo(const QString &);
  void setFiguresInfo2(const QString &);
};

#endif // SCENE_H
