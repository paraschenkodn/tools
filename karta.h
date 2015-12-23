#ifndef KARTA_H
#define KARTA_H

#include "level.h"

#include "mapbuilder.h"

/// примитивы
#include "shphere.h"
#include "pointsofsphere.h"
#include "text.h"

#include <QOpenGLTexture>
#include <QOpenGLShaderProgram>  // объявляем для передачи шейдерных программ в объект


class Karta : public QObject
{
  Q_OBJECT // для связывания сигналов и слотов, : public QObject тоже для этого
public:
  Karta();
  ~Karta();

  // ДВИЖОК РИСОВАНИЯ КАРТЫ
  MapBuilder *mbuilder;   // класс картостроителя
  void kartaInit();
  QOpenGLShaderProgram program;   // шейдерная программа рисования линий
  QVector<QOpenGLShaderProgram *> programs;
  // создаём идентификаторы для обращения к шейдерным переменным
  int m_vertexAttr;
  int m_colorAttr;
  int m_matrixUniform;
  int m_texAttr;
  int m_texUniform;
  int m_MVPmatrix;
  void set();
  void draw(int draw_law); // рисование в режиме построения карты (InterNet-Router-SubNet-LAN-Node)
  void reset();
  void paintSphereMap(); // рисование сферической карты
  void paintFlatMap(int draw_law); // рисование плоской карты

  // model part
  QVector<void *> primitives;

  // primitives part
  shphere *m_shphere;
  Text *m_text;
  struct simple_map {
    std::vector<float> vertices; // создаём вектор (контейнер хранения) координат вершин
    std::vector<float> m_colors; // создаём вектор (контейнер хранения) координат цветов
    QVector<QString> captions; // список названий
    float IDm;    // идентификатор модели
    std::vector<GLfloat> IDv; // создаём вектор (контейнер хранения) идентификаторов вершин (номер модели, индекс вершины, 0, признак выборки)
  } smap;

  shphere *shpheres;
  Text *texts;

  Level *level;
public slots:
  /// интерфейс получения данных от плагина-картостроителя (id "BuilderMapInterface")
  void buildMapFromPlugin();
};

#endif // KARTA_H
