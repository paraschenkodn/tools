#include "text.h"
#include <QFontMetrics>

Text::Text()
{
  // создаём переменные по умолчанию
  pen.setColor(QColor(255,255,255,255));
  // грузим текстуру test
  //m_texture=new QOpenGLTexture(QImage(":/Textures/Blocks.jpg"));

  // инициализируем шейдеры
  QOpenGLShader vShader(QOpenGLShader::Vertex);
  vShader.compileSourceFile(":/Shaders/vTextO.glsl");
  QOpenGLShader fShader(QOpenGLShader::Fragment);
  fShader.compileSourceFile(":/Shaders/fTextO.glsl");
  //добавляем шейдеры в программу
  program.addShader(&vShader);
  program.addShader(&fShader);
  // линкуем загруженные в программу шейдеры вместе и проверяем
  if (!program.link()){
      qWarning("Text. Шейдерная программа не слинковалась.");
      return;
  }
  m_vertexAttr=program.attributeLocation("vertexAttr");
  m_colorAttr=program.attributeLocation("colorAttr");
  m_MVPmatrix=program.uniformLocation("MVPM");
  m_texAttr=program.attributeLocation("texAttr");
  m_texUniform=program.attributeLocation("texUniform");//*/
}

void Text::set()
{
  //подключаем программу и проверяем
  if (!program.bind()){
      qWarning("Text. Шейдерная программа не сбиндилась");
      return;
  }
  // убираем искажения текстур
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void Text::drawO(const QMatrix4x4 & mvpmatrix, const QString &_str, const QVector3D &coords)
    {
        if (_str.isEmpty())
            return;

        QFontMetrics fm(font);
        QRect rect = fm.boundingRect(_str); // text bounding box
        rect.adjust(0, 0, 1, 1);            // прибавили по единице ко второй точке

        QImage image(rect.size(), QImage::Format_ARGB32);
        image.fill(0); // set to transparent

        // Draw the text on an image by QPainter
        p2d. begin(&image);
        p2d. setFont(font);
        p2d. setPen(pen);
        p2d. drawText(0, 0, rect.width(), rect.height(), Qt::AlignCenter, _str);
        p2d. end();

        // Get viewport *************** получаем размер поля рисования, чтобы его использовать как ортогональную проекцию
        // для рисования текста в ней
        GLint view[4];
        glGetIntegerv(GL_VIEWPORT, view);

        // Convert the object coordinate to screen coordinate
        QVector3D win=mvpmatrix.map(coords); // получили координаты в нормализованных к -1,1 координатах
        win.setX((win.x()+1)*view[2]/2); // получили в координатах окна показа viewport
        win.setY((win.y()+1)*view[3]/2);

        // Define the font rectangle
        int x = (int) win.x(), y = (int) win.y();

        if (align & Qt::AlignLeft){
            x -= rect.width();
        }
        if (align & Qt::AlignTop){
            y -= rect.height();
        }
        if (align & Qt::AlignHCenter){
            x -= rect.width() / 2;
        }
        if (align & Qt::AlignVCenter){
            y -= rect.height() / 2;
        }

        vertices.resize(8);
        vertices[0]=x; vertices[1]=y;
        vertices[2]=x; vertices[3]=y + rect.height();
        vertices[4]=x + rect.width(); vertices[5]=y + rect.height();
        vertices[6]=x + rect.width(); vertices[7]=y;

        texCoord.resize(8);
        texCoord[0]=0.0f; texCoord[1]=1.0f; texCoord[2]=0.0f; texCoord[3]=0.0f;
        texCoord[4]=1.0f; texCoord[5]=0.0f; texCoord[6]=1.0f; texCoord[7]=1.0f;

        // Map the image to texture
        QOpenGLTexture texture(image);

        // Use ortho projection to draw the text
        QMatrix4x4 projm;
        projm.ortho(view[0], view[2], view[1], view[3], 0, 1);

        // Enable blend to make the background transparency of the text
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

        texture.bind();

        // translate data to shader
        program.setAttributeArray(m_vertexAttr, vertices.data(), 2);
        //m_program.setAttributeArray(m_colorAttr, m_colors.data(), 3);
        program.setAttributeArray(m_texAttr, texCoord.data(), 2);
        program.setUniformValue(m_texUniform,0);

        // активируем массивы
        program.enableAttributeArray(m_vertexAttr);
        //m_program.enableAttributeArray(m_colorAttr);
        program.enableAttributeArray(m_texAttr);//*/

        program.setUniformValue(m_MVPmatrix, projm);

        // рисуем треугольник
        glDrawArrays(GL_TRIANGLE_FAN,0,4);

        // деактивируем массивы
        program.disableAttributeArray(m_vertexAttr);
        //program.disableAttributeArray(m_colorAttr);
        program.disableAttributeArray(m_texAttr);

        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_BLEND);
}

void Text::reset()
{
  // очищаем программу
  program.release();
}

