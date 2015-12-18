#ifndef SCENE_H
#define SCENE_H

#include <QOpenGLWidget>
/// ---- level ----
#include "level.h"
#include "cameraquat.h"
/// ---- model ----
#include "karta.h"
#include "mapbuilder.h"
/// ---- primitives ----
#include "shphere.h"
#include "pointsofsphere.h"
#include "text.h"
#include "triangle.h"

#include <QOpenGLShaderProgram>
#include <QKeyEvent>
#include <QTimer>
#include <QOpenGLFunctions>
#include <QOpenGLFunctions_3_0>
#include <QOpenGLFramebufferObject>

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
  float near_, far_, range;
  QOpenGLFunctions_3_0 *f;

  // level part
  CameraQuat camera;
  QVector<void *> models;

  // модели
  Karta *karta;
  QVector<void *> primitives;

  // примитивы
  Triangle *m_triangle;
  shphere *m_shphere;
  pointsofsphere *spherepoints;
  Text *m_text;

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

  // переменные выборки объектов в шейдерах
  QVector4D selectID;   // выбранный объект (ID модели, ID vertex, 0, result) if result==3711 - nothing selected
  QVector4D oldSelectID;
  bool selectmode;  // true - выбираем в следующем кадре объект в шейдере, false - нет
  bool preSelectMode;  // true - ожидание подтверждения включения выборки, false - нет
  QPointF pmouse;    // точка выборки

  // TEST
  void setSelected();

private:
  void initializeGL();
  void paintGL();
  void paintDM(); // рисование в режиме разработчика
  void resizeGL(int w, int h);

  // определяем метод изменяющий координаты положения камеры
  float mouse_sensitivity;
  QPointF pixelPosToViewPos(const QPointF &p);
  void keyPressEvent(QKeyEvent *event);         // управление с клавиатуры
  void mouseDoubleClickEvent(QMouseEvent * event); // выбор объектов
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

  QTimer m_timer; // таймер анимации

  void setLights();
  void setStates();
  void defaultStates();

  bool perspective;  // признак рисования в перспективной или в ортогональной проекции
  int paintMode; // режим рисования

  // буфера
  GLuint vbo; // количество свободных буферов VBO
  //QOpenGLFramebufferObject *fbo;

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
