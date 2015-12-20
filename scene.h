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

  QOpenGLFunctions_3_0 *f;

  // levels
  std::vector<Level *> levels;  // основной объект сцены
  Level * currentLevel;   // viewed scene
  // level part
  CameraQuat camera;
  QVector<void *> models;
  int angle_z; // текущий угол поворота камеры по Z

  // модели
  Karta *karta;
  QVector<void *> primitives;

  // примитивы
  Triangle *m_triangle;
  shphere *m_shphere;
  pointsofsphere *spherepoints;
  Text *m_text;

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
  void paintKarta(int draw_law); // рисование в режиме разработчика
  void resizeGL(int w, int h);

  // определяем метод изменяющий координаты положения камеры
  float mouse_sensitivity;
  QPointF pixelPosToViewPos(const QPointF &p);
  void keyPressEvent(QKeyEvent *event);         // управление с клавиатуры
  void mouseDoubleClickEvent(QMouseEvent * event); //
  void mousePressEvent(QMouseEvent *event);      // работка с мышью, переопределяем функции обработки сообщений мыши (нажатие кнопок)
  void mouseReleaseEvent(QMouseEvent *event);    // переопределяем функции обработки сообщений мыши (отпускание кнопки)
  ///*********
  void mouseMoveEvent(QMouseEvent *event);       // переопределяем функции обработки сообщений мыши (перемещение)
  ///* если будем водить мышью по форме диалога с нажатой клавишей, сообщения также получает сцена
  /// ********
  void wheelEvent(QWheelEvent * event);  // переопределяем функции обработки сообщений мыши (колесо)

  // переменные фигур
  float step; // шаг сдвига фигур

  void setFigureInfo(); // формируем и посылаем текст для отображения параметров в главном окне

  QTimer m_timer; // таймер анимации

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
  void reset();
private slots:
  // заставляем прорисовываться по таймеру
  void slotAnimation();

signals:
  void setFiguresInfo(const QString &);
  void setFiguresInfo2(const QString &);
  void setBar(const QString &);
};

#endif // SCENE_H
