#include "karta.h"
#include "mainwindow.h"

Karta::Karta()
{
    /// подготавливаем прорисовку линий
    // инициализируем шейдеры
    QOpenGLShader vShader(QOpenGLShader::Vertex);
    vShader.compileSourceFile(":/Shaders/vShader.glsl");
    QOpenGLShader fShader(QOpenGLShader::Fragment);
    fShader.compileSourceFile(":/Shaders/fShader.glsl");
    program.addShader(&vShader);
    program.addShader(&fShader);
    if (!program.link()){
        qWarning("KARTA. Line. Шейдеры не слинковалась");
        return;
    }
    m_vertexAttr=program.attributeLocation("vertexAttr");
    m_MVPmatrix=program.uniformLocation("MVPM");

    // порождаем для карты свои примитивы
    //shphere shpheres=new shphere();
    //Text texts=new Text();
}

Karta::~Karta()
{
// удаление делается в деструкторе сцены, где уже использован MakeCurrent(), поэтому пропускаем
    //delete shpheres;
    //delete texts;
}

void Karta::set()
{
    return;
}

void Karta::draw()
{/*
  // РИСУЕМ ЛИНИИ
    //подключаем программу и проверяем
    if (!program.bind()){
        qWarning("KARTA. Line. Шейдеры не сбиндились");
        return;
    }

    // устанавливаем место хранения координат
      program.setAttributeArray(m_vertexAttr, vertices.data(), 3);

      // активируем массивы
      program.enableAttributeArray(m_vertexAttr);

      // зaпихиваем в его программу матрицу ориентации
      program.setUniformValue(m_MVPmatrix, ui->SceneWidget.MVPM);

      // устанавливаем цвет линий
      glColor3i(0,255,0);
      //glLineWidth(10);
      // рисуем линии
      glDrawArrays(GL_LINES,0,vertices.size()/3);

      // деактивируем массивы
      program.disableAttributeArray(m_vertexAttr);

      // очищаем программу
      program.release();

  //РИСУЕМ СФЕРЫ
  shpheres->setPerspective();   // устанавливаем режим рисования сфер в перспективной проекции
  shpheres->init();             //
  shpheres->m_program->setUniformValue(shpheres->m_matrixUniform, MVPM);
  shpheres->m_program->setUniformValue("PMi", PMi);                          // TODO вынести в класс шфер
  shpheres->m_program->setUniformValue("MVM", MVM);
  shpheres->m_program->setUniformValue("MVPMi", MVPMi);
  shpheres->m_program->setUniformValue("viewport",viewport);

  shpheres->draw();

  shphere->drop();///

  //РИСУЕМ ТЕКСТ
  //m_text->font=QFont::Bold;
  //m_text->pen=
  m_text->set();
  m_text->drawO(MVPM,"Точка 1",QVector3D(m_triangle->m_vertices[0],m_triangle->m_vertices[1],m_triangle->m_vertices[2]));
  m_text->drawO(MVPM,"Точка 2",QVector3D(m_triangle->m_vertices[3],m_triangle->m_vertices[4],m_triangle->m_vertices[5]));
  m_text->drawO(MVPM,"Точка 3",QVector3D(m_triangle->m_vertices[6],m_triangle->m_vertices[7],m_triangle->m_vertices[8]));
  m_text->reset();
//*/
  return;
}

