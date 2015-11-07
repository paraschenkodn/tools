#include "text.h"
#include <QFontMetrics>

Text::Text()
{
  // создаём переменные по умолчанию
  //font(); pen();
  pen.setColor(QColor(255,255,255,255));
  // прямоугольник рисования
  // текстура
  // переменная рисовальщика
  //p2p();

  // грузим текстуру
  m_texture=new QOpenGLTexture(QImage(":/Textures/rbt.png"));

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

void Text::init()
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
        rect.adjust(0, 0, 1, 1);            // normalize bounding box

        QImage image(rect.size(), QImage::Format_ARGB32);
        image.fill(0); // set to transparent

        // Draw the text on an image
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
        //GLdouble winx, winy, winz; // the screen coordinate of the object
        //model = painter->modelViewMatrix().top();
        //proj = painter->projectionMatrix().top();
        //GLdouble modelMatrix[16];
        //for (int i = 0; i < 16; i++)
            //modelMatrix[i] = model.data()[i];
        //GLdouble projMatrix[16];
        //for (int i = 0; i < 16; i++)
            //projMatrix[i] = proj.data()[i];
        //gluProject(coords.x(), coords.y(), coords.z(),
            //modelMatrix, projMatrix, view,
            //&winx, &winy, &winz);
        QVector3D win=mvpmatrix.mapVector(coords);

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

        //QVector2DArray vertices;
        //vertices.append(x, y);
        //vertices.append(x, y + rect.height());
        //vertices.append(x + rect.width(), y + rect.height());
        //vertices.append(x + rect.width(), y);//
        vertices.resize(8);
        vertices[0]=x; vertices[1]=y;
        vertices[2]=x; vertices[3]=y + rect.height();
        vertices[4]=x + rect.width(); vertices[5]=y + rect.height();
        vertices[6]=x + rect.width(); vertices[7]=y;

        // Texture coordinates
        //QVector2DArray texCoord;
        //texCoord.append(0.0f, 0.0f);
        //texCoord.append(0.0f, 1.0f);
        //texCoord.append(1.0f, 1.0f);
        //texCoord.append(1.0f, 0.0f);//
        texCoord.resize(8);
        texCoord[0]=0.0f; texCoord[1]=0.0f; texCoord[2]=0.0f; texCoord[3]=1.0f;
        texCoord[4]=1.0f; texCoord[5]=1.0f; texCoord[6]=1.0f; texCoord[7]=0.0f;

        // Map the image to texture
        //QGLTexture2D texture;
        //texture.setImage(image);
        QOpenGLTexture texture(image);

        // Set projection matrix stack
        //painter->modelViewMatrix().push();
        //painter->modelViewMatrix().setToIdentity();

        // Use ortho projection to draw the text
        QMatrix4x4 projm;
        projm.ortho(view[0], view[2], view[1], view[3], 0, 1);

        //painter->projectionMatrix().push();
        //painter->projectionMatrix() = projm;

        // Enable blend to make the background transparency of the text
        //glEnable(GL_BLEND);
        //glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

        //painter->clearAttributes();
        //painter->setStandardEffect(QGL::FlatReplaceTexture2D);

        texture.bind();
        //painter->setVertexAttribute(QGL::Position, vertices);
        //painter->setVertexAttribute(QGL::TextureCoord0, texCoord);

        // translate data
        program.setAttributeArray(m_vertexAttr, vertices.data(), 2);
        //m_program.setAttributeArray(m_colorAttr, m_colors.data(), 3);
        program.setAttributeArray(m_texAttr, texCoord.data(), 2);
        program.setUniformValue(m_texUniform,0);

        // активируем массивы
        program.enableAttributeArray(m_vertexAttr);
        //m_program.enableAttributeArray(m_colorAttr);
        program.enableAttributeArray(m_texAttr);//*/

        program.setUniformValue(m_MVPmatrix, projm);

        //painter->draw(QGL::TriangleFan, 4);
        // рисуем треугольник
        glDrawArrays(GL_TRIANGLE_FAN,0,4);

        // деактивируем массивы
        program.disableAttributeArray(m_vertexAttr);
        //program.disableAttributeArray(m_colorAttr);
        program.disableAttributeArray(m_texAttr);

        //painter->setStandardEffect (QGL::FlatColor);
        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_BLEND);

        // Restore the matrix stack
        //painter->projectionMatrix().pop();
        //painter->modelViewMatrix().pop();
        //*/
}

void Text::drop()
{
  // очищаем программу
  program.release();
}

