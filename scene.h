#ifndef SCENE_H
#define SCENE_H

#include <QOpenGLWidget>
#include "triangle.h"
#include "pointsofsphere.h"
#include "shphere.h"
#include "text.h"
#include "mapbuilder.h"
#include "karta.h"
#include "cameraquat.h"

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

  // примитивы
  Triangle *m_triangle;
  shphere *m_shphere;
  pointsofsphere *spherepoints;
  Text *m_text;
  Karta *karta;
  CameraQuat camera;

  // матрицы преобразований
  QMatrix4x4 LMM; // Local Model matrix (делает преобразования в локальных координатах объекта, для одного объекта их может быть несколько для разных частей объекта)
  QMatrix4x4 MM; // Model matrix (выносит координаты объекта в координаты пространства сцены,
                //выполняется в следующем порядке - масштабирование, поворот, перенос)
                // TranslationMatrix * RotationMatrix * ScaleMatrix * OriginalVector; (в коде это выглядит в обратном порядке)
  QMatrix4x4 MVM; //WorldView; // ModelView matrix (View matrix)("масштабирует крутит и перемещает весь мир")
  QMatrix4x4 CameraView; // тоже самое что и MVM, но для использования функции LookAt
  QMatrix4x4 PM; // Projection matrix // проекционная матрица
  QMatrix4x4 MVPM; // ModelViewProjection matrix (projection * view * model)
  // инверсные матрицы
  QMatrix4x4 PMi;
  QMatrix4x4 MVPMi;

private:
  void initializeGL();
  void paintGL();
  void paintDM(); // рисование в режиме разработчика
  void resizeGL(int w, int h);

  // определяем метод изменяющий координаты положения камеры
  float mouse_sensitivity;
  QPointF pixelPosToViewPos(const QPointF &p);
  void keyPressEvent(QKeyEvent *event);         // управление с клавиатуры
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
  QQuaternion cameraOrientation; // итоговый вектор ориентации камеры
  float cameraFocusAngle;
  //int camEyePos; // позиция камеры () // + control of board??
  void setCamera();  // установка параметров матрицы CameraView
  void setCameraInfo();  // формируем и посылаем текст для отображения параметров в главном окне
  int angle_x; // текущий угол поворота камеры по X (матрицы поворота CameraView, она же ModelView)
  int angle_y; // текущий угол поворота по Y
  int angle_z; // текущий угол поворота по Z
  // енд камера

  void setFigureInfo(); // формируем и посылаем текст для отображения параметров в главном окне

  QTimer m_timer;

  void setLights();
  void setStates();
  void defaultStates();

  bool perspective;  // признак рисования в перспективной или в ортогональной проекции
  int paintMode; // режим рисования

  // ДВИЖОК РИСОВАНИЯ КАРТЫ
  MapBuilder *mbuilder;   // класс картостроителя
  void kartaInit();
  void paintKarta(); // рисование в режиме построения карты (InterNet-Router-SubNet-LAN-Node)
  void paintFlatMap(); // рисование плоской карты
  void paintSphereMap(); // рисование сферической карты
  QOpenGLShaderProgram m_program;   // шейдерная программа рисования линий
  QVector<QOpenGLShaderProgram *> programs;
  // создаём идентификаторы для обращения к шейдерным переменным
  int m_vertexAttr;
  int m_colorAttr;
  int m_matrixUniform;
  int m_texAttr;
  int m_texUniform;

public slots:
  // устанавливаем признак рисования в перспективной или ортогональной проекции
  void setPerspective(int _switch);
  // Установка режима рисования
  void setPaintMode(int mode);  // 1 - test, 2 - karta
  // строим новую карту
  void buildNewMap();
  // добавляем в карту
  void addMap();
  void reset();
private slots:
  // заставляем прорисовываться по таймеру
  void slotAnimation();

signals:
  void setPerspectiveInfo(const QString &);
  void setFiguresInfo(const QString &);
  void setFiguresInfo2(const QString &);
  void setBar(const QString &);
};

#endif // SCENE_H
