#ifndef TEXT_H
#define TEXT_H

#include <QOpenGLTexture>
#include <QOpenGLShaderProgram>  // объявляем для передачи шейдерных программ в объект

class Text
{
public:
    Text();
    void init();
    void draw(); // рисуем сферу (шейдерами)
    void drop();

    std::vector<float> vertices; //создаём вектор (контейнер хранения) координат вершин
    std::vector<float> m_colors; //создаём вектор (контейнер хранения) координат цветов
    std::vector<float> texCoord; // координаты текстуры

};

#endif // TEXT_H
