#ifndef KARTA_H
#define KARTA_H

#include <QOpenGLTexture>
#include <QOpenGLShaderProgram>  // объявляем для передачи шейдерных программ в объект

class Karta
{
public:
  Karta();
  void set();
  void draw(); // рисуем карту
  void reset();

  std::vector<float> vertices; // создаём вектор (контейнер хранения) координат вершин
  std::vector<float> m_colors; // создаём вектор (контейнер хранения) координат цветов
  std::vector<float> texCoord; // координаты текстуры
  QVector<QString> captions; // список названий

  QOpenGLShaderProgram program;
  int m_vertexAttr;
  int m_colorAttr;
  int m_texAttr;
  int m_texUniform;
  int m_MVPmatrix;
};

#endif // KARTA_H
