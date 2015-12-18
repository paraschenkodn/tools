#ifndef KARTA_H
#define KARTA_H

#include "scene.h"
#include "level.h"

#include "mapbuilder.h"

/// примитивы
#include "shphere.h"
#include "pointsofsphere.h"
#include "text.h"

#include <QOpenGLTexture>
#include <QOpenGLShaderProgram>  // объявляем для передачи шейдерных программ в объект


class Karta
{
public:
  Karta();
  ~Karta();

  void set();
  void draw(); // рисуем карту
  void reset();

  // model part
  QVector<void *> primitives;

  std::vector<float> vertices; // создаём вектор (контейнер хранения) координат вершин
  std::vector<float> m_colors; // создаём вектор (контейнер хранения) координат цветов
  std::vector<float> texCoord; // координаты текстуры
  QVector<QString> captions; // список названий
  float IDm;    // идентификатор модели
  std::vector<GLfloat> IDv; // создаём вектор (контейнер хранения) идентификаторов вершин (номер модели, индекс вершины, 0, признак выборки)

  QOpenGLShaderProgram program; // шейдер рисования линий
  int m_vertexAttr;
  int m_colorAttr;
  int m_texAttr;
  int m_texUniform;
  int m_MVPmatrix;

  shphere *shpheres;
  Text *texts;

  void paintFlatMap();
};

#endif // KARTA_H
