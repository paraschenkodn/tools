#ifndef TEXT_H
#define TEXT_H

#include <QPainter>

#include <QOpenGLTexture>
#include <QOpenGLShaderProgram>  // объявляем для передачи шейдерных программ в объект

class Text
{
public:
    Text();
    void set();
    void drawO(const QMatrix4x4 & mvpmatrix, const QString &_str, const QVector3D &coords); // рисуем текст в ортогональной проекции (поверх основной сцены)
    void reset();

    // TODO
    //void init();  // загрузка шрифтов в буфер ОГЛ
    //void drawSHP(QString str); // рисуем text (шейдерами) perspective projection
    //void drop();

    QPainter p2d;  // paint device (рисовальщик нашей текстуры)
    QFont font;   // фонт которым мы рисуем (инициализируется системным по умолчанию)
    QPen pen;   // перо которым мы рисуем (инициализируется системным по умолчанию)
    Qt::Alignment align;  // центровка текста

    std::vector<float> vertices; //создаём вектор (контейнер хранения) координат вершин
    std::vector<float> m_colors; //создаём вектор (контейнер хранения) координат цветов
    std::vector<float> texCoord; // координаты текстуры

    QOpenGLShaderProgram program;
    int m_vertexAttr;
    int m_colorAttr;
    int m_texAttr;
    int m_texUniform;
    int m_MVPmatrix;

    QOpenGLTexture *m_texture;
};

#endif // TEXT_H
