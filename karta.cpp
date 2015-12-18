#include "karta.h"
#include "mainwindow.h"
#include "shphere.h"
#include "text.h"

Karta::Karta()
{
    /// подготавливаем прорисовку линий
    // инициализируем шейдеры
    QOpenGLShader vShader(QOpenGLShader::Vertex);
    vShader.compileSourceCode("attribute highp vec4 vertexAttr;" // входной аттрибут для координат вершин
                              "uniform mediump mat4 MVPM;"        // матрица положения средней точности задающая позицию вершин
                              "void main () {"
                                  "gl_Position = MVPM * vertexAttr;" // устанавливаем позицию вершин через матрицу
                              "}");
    QOpenGLShader fShader(QOpenGLShader::Fragment);
    fShader.compileSourceCode("uniform lowp vec4 statcolor;"  // статичный цвет рисования (aka glColor)
                              "void main () {"
                                  "gl_FragColor = statcolor;" // просто принимаем значение без обработки
                              "}");
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
{    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
     PM.setToIdentity();
     PM.perspective(cameraFocusAngle,ratio,near_,far_);  // glFrustum( xmin, xmax, ymin, ymax, near, far)  // gluPerspective(fovy, aspect, near, far)
     MVPM=PM*MVM;

     // находим проекционную инверсную мтрицу
     bool inverted;
     PMi=PM.inverted(&inverted);
     if (!inverted)
         qDebug() << "PMi не конвертится";
     MVPMi=MVPM.inverted(&inverted);
     if (!inverted)
         qDebug() << "MVPMi не конвертится";


     if (mbuilder->currentmap==FLAT_MAP) paintFlatMap();
}

void Karta::paintFlatMap()
{
  //проверяем, готовы ли данные, если да, то копируем для отображения
  if (mbuilder->newmapbuild)  { /// TODO Перевести в сигнал  (и в модуль карты)
      mbuilder->m_mutex.lock();
      vertices.assign(mbuilder->vertices.begin(),mbuilder->vertices.end()); // copy vertices
      captions=mbuilder->captions; // copy captions
      m_colors.resize(mbuilder->vertices.size());   //
      for (uint i=0;i<m_colors.size();i+=3) {
          m_colors[i]=1.0f;
          m_colors[i+1]=0.0f;
          m_colors[i+2]=0.0f;
      }
      mbuilder->newmapbuild=false; // забрали карту, готовьте новую
      mbuilder->m_mutex.unlock();

      // готовим данные для выборки объектов (TODO убрать ограничение в 255 объектов)
      IDv.resize(captions.size()*4);
      float index=0.001f;                    // index должен соответствовать функции setSelected()
      for (int i=0;i<captions.size();i++) {
          IDv[i*4]=IDm; // ID model
          IDv[i*4+1]=i*index; //((float)i/(float)karta->captions.size());    // ID point ДЕЛЁННОЕ НА КОЛИЧЕСТВО ФРАГМЕНТОВ
          IDv[i*4+2]=0;    // reserv
          IDv[i*4+3]=1.0f;  //
      }
    }

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
    // зaпихиваем в программу матрицу ориентации
    program.setUniformValue(m_MVPmatrix, MVPM);
    // устанавливаем цвет линий
    //karta->program.setUniformValue("colormode", true);    // рисуем одним цветом, не массивом
    program.setUniformValue("statcolor", QVector4D(0.0f,1.0f,0.0f,1.0f));  //
    // рисуем линии
    glDrawArrays(GL_LINES,0,vertices.size()/3);
    // деактивируем массивы
    program.disableAttributeArray(m_vertexAttr);
    // очищаем программу
    program.release();

      //РИСУЕМ СФЕРЫ
      m_shphere->setPerspective();   // устанавливаем режим рисования сфер в перспективной проекции
      m_shphere->init();             // bind program
      m_shphere->m_program->setUniformValue(m_shphere->m_matrixUniform, MVPM);
      m_shphere->m_program->setUniformValue("PM", PM);                          // TODO вынести в класс шфер
      m_shphere->m_program->setUniformValue("PMi", PMi);                          // TODO вынести в класс шфер
      m_shphere->m_program->setUniformValue("MVM", MVM);
      m_shphere->m_program->setUniformValue("MVPMi", MVPMi);
      m_shphere->m_program->setUniformValue("viewport",viewport);
      //m_shphere->m_program->setUniformValue("near", near_);                          // TODO вынести в класс шфер
      //m_shphere->m_program->setUniformValue("far", far_);                          // TODO вынести в класс шфер
      m_shphere->m_program->setUniformValue("range", range);                          // TODO вынести в класс шфер

      //shpheres->draw();
      m_shphere->m_program->setAttributeArray(m_shphere->m_vertexAttr, vertices.data(), 3);
      m_shphere->m_program->setAttributeArray(m_shphere->m_colorAttr, m_colors.data(), 3);
      m_shphere->m_program->setUniformValue("R",m_shphere->radius);
      m_shphere->m_program->setUniformValue("maxpointsize",m_shphere->maxpointsize);
      // активируем массивы
      m_shphere->m_program->enableAttributeArray(m_shphere->m_vertexAttr);
      m_shphere->m_program->enableAttributeArray(m_shphere->m_colorAttr);
      // активируем выборку объектов
      m_shphere->m_program->setAttributeArray("selectID", IDv.data(), 4);
      m_shphere->m_program->enableAttributeArray("selectID");
      m_shphere->m_program->setUniformValue("selectmode", selectmode);    // указываем режим выборки
      // рисуем точки
      glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);   // говорим что будут меняться размеры точек в шейдере
      glDrawArrays(GL_POINTS,0,vertices.size()/3);
      glDisable(GL_VERTEX_PROGRAM_POINT_SIZE);
      // производим проверку выборки объектов
      m_shphere->m_program->disableAttributeArray("selectID");
      // деактивируем массивы
      m_shphere->m_program->disableAttributeArray(m_shphere->m_vertexAttr);
      m_shphere->m_program->disableAttributeArray(m_shphere->m_colorAttr);
      m_shphere->drop();    // release program

      //РИСУЕМ ТЕКСТ
      //m_text->font=QFont::Bold;
      //m_text->pen=
      m_text->set();
      for (int i=0;i<captions.size();i++) {
          m_text->drawO(MVPM,captions[i],QVector3D(vertices[i*3],vertices[(i*3)+1],vertices[(i*3)+2]));
      }
      m_text->reset();
}
