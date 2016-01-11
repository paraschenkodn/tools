#include "karta.h"
//#include "mainwindow.h"
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

    mbuilder=new MapBuilder();  // экземпляр картостроителя для текущей карты

    // порождаем для карты свои примитивы
    m_shphere=new shphere();
    m_text =new Text();

    plugin_data=false;
}

Karta::~Karta()
{
// удаление делается в деструкторе сцены, где уже использован MakeCurrent(), поэтому пропускаем MakeCurrent()
    delete mbuilder;
  delete m_shphere;
  delete m_text;
}

void Karta::set()
{
    return;
}

void Karta::draw(int draw_law)
{
if (mbuilder->currentmap==FLAT_MAP || mbuilder->currentmap==FLAT_MAP) paintFlatMap(draw_law);
}

void Karta::paintFlatMap(int draw_law)
{
  //проверяем, готовы ли данные, если да, то копируем для отображения
  if (mbuilder->newmapbuild)  { /// TODO Перевести в сигнал  (и в модуль карты)
      mbuilder->m_mutex.lock();
      smap.vertices.assign(mbuilder->vertices.begin(),mbuilder->vertices.end()); // copy vertices
      smap.captions=mbuilder->captions; // copy captions
      smap.m_colors.resize(mbuilder->vertices.size());   //
      for (uint i=0;i<smap.m_colors.size();i+=3) {
          smap.m_colors[i]=1.0f;
          smap.m_colors[i+1]=0.0f;
          smap.m_colors[i+2]=0.0f;
      }
      mbuilder->newmapbuild=false; // забрали карту, готовьте новую
      mbuilder->m_mutex.unlock();

      // готовим данные для выборки объектов
      smap.IDv.resize(smap.captions.size()*4);
      float index=0.001f;                    // index должен соответствовать коэфициенту в функции setSelected()
      for (int i=0;i<smap.captions.size();i++) {
          smap.IDv[i*4]=smap.IDm; // ID model
          smap.IDv[i*4+1]=i*index; //((float)i/(float)karta->captions.size());    // ID point ДЕЛЁННОЕ НА КОЛИЧЕСТВО ФРАГМЕНТОВ
          smap.IDv[i*4+2]=0;    // reserv
          smap.IDv[i*4+3]=1.0f;  //
      }
  }

  // РИСУЕМ ЛИНИИ
  if (draw_law==DRAW_NORM) {
    //подключаем программу и проверяем
    if (!program.bind()){
        qWarning("KARTA. Line. Шейдеры не сбиндились");
        return;
    }
    // устанавливаем место хранения координат
      program.setAttributeArray(m_vertexAttr, smap.vertices.data(), 3);
      // активируем массивы
      program.enableAttributeArray(m_vertexAttr);
      // зaпихиваем в программу матрицу ориентации
      program.setUniformValue(m_MVPmatrix, level->MVPM);
      // устанавливаем цвет линий
      //karta->program.setUniformValue("colormode", true);
      program.setUniformValue("statcolor", QVector4D(0.0f,1.0f,0.0f,1.0f));  // рисуем одним цветом, не массивом
      // рисуем линии
      glDrawArrays(GL_LINES,0,smap.vertices.size()/3);  // режим рисования попарно для точек
      // деактивируем массивы
      program.disableAttributeArray(m_vertexAttr);
      // очищаем программу
      program.release();
  }
      //РИСУЕМ СФЕРЫ
      bool selectmode=false;
      if (draw_law==DRAW_SFBO) {
          selectmode=true;
        }
      m_shphere->setPerspective();   // устанавливаем режим рисования сфер в перспективной проекции
      m_shphere->init();             // bind program
      m_shphere->m_program->setUniformValue(m_shphere->m_matrixUniform, level->MVPM);
      m_shphere->m_program->setUniformValue("PM", level->PM);                 // TODO вынести в класс шфер
      m_shphere->m_program->setUniformValue("PMi", level->PMi);                 // TODO вынести в класс шфер
      m_shphere->m_program->setUniformValue("MVM", level->MVM);
      m_shphere->m_program->setUniformValue("MVPMi", level->MVPMi);
      m_shphere->m_program->setUniformValue("viewport",level->viewport);

      //shpheres->draw();
      m_shphere->m_program->setAttributeArray(m_shphere->m_vertexAttr, smap.vertices.data(), 3);
      m_shphere->m_program->setAttributeArray(m_shphere->m_colorAttr, smap.m_colors.data(), 3);
      m_shphere->m_program->setUniformValue("R",m_shphere->radius);
      m_shphere->m_program->setUniformValue("maxpointsize",m_shphere->maxpointsize);
      // активируем массивы
      m_shphere->m_program->enableAttributeArray(m_shphere->m_vertexAttr);
      m_shphere->m_program->enableAttributeArray(m_shphere->m_colorAttr);
      // активируем выборку объектов
      m_shphere->m_program->setAttributeArray("selectID", smap.IDv.data(), 4);
      m_shphere->m_program->enableAttributeArray("selectID");
      m_shphere->m_program->setUniformValue("selectmode", selectmode);    // указываем режим выборки
      // рисуем точки
      glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);   // говорим что будут меняться размеры точек в шейдере
      glDrawArrays(GL_POINTS,0,smap.vertices.size()/3);
      glDisable(GL_VERTEX_PROGRAM_POINT_SIZE);
      // производим проверку выборки объектов
      m_shphere->m_program->disableAttributeArray("selectID");
      // деактивируем массивы
      m_shphere->m_program->disableAttributeArray(m_shphere->m_vertexAttr);
      m_shphere->m_program->disableAttributeArray(m_shphere->m_colorAttr);
      m_shphere->drop();    // release program

      //РИСУЕМ ТЕКСТ
  if (draw_law==DRAW_NORM) {
      //m_text->font=QFont::Bold;
      //m_text->pen=
      m_text->set();
      for (int i=0;i<smap.captions.size();i++) {
          m_text->drawO(level->MVPM,smap.captions[i],QVector3D(smap.vertices[i*3],smap.vertices[(i*3)+1],smap.vertices[(i*3)+2]));
      }
      m_text->reset();
    }
}

void Karta::buildFromPlugin(QAction *act)  // работа с интерфейсом плагина
{
  //mbuilder->newmapbuild=false;    // организовываем построение карты
  //pact = qobject_cast<QAction*>(sender());
  pact=act;
  pI = qobject_cast<BuilderMapInterface*>(pact->parent());
  mapFromPlugin(-1); ///TODO надо сделать для текущего плагина запоминание idV
}

void Karta::mapFromPlugin(int idV)  // работа с интерфейсом плагина
{
  plugin_data=true;
  ///TODO надо сделать для текущего плагина запоминание idV
  if (idV==-1) { // при -1 восстанавливаем запомненную позицию выбора
      ;
    }
  //int ID=0; // показываем что строится с нуля (первая карта по умолчанию всегда строится сама с нуля)
  /// построение карты из файла, начиная с нода==ID, передаём в плагин название операции и параметры
  /// плагин должен вернуть:
  /// 1. указатель на массив вершин
  /// 2. указатель на массив идентификаторов вершин (например названия)
  //pI->newbmap(pact->text(),ID,&smap.vertices,&smap.captions);

  pI->newbmap(pact->text(),idV);

  // переписываем данные для отображения
  if (/*mbuilder->newmapbuild*/1)  {
      //mbuilder->m_mutex.lock();
      smap.vertices.assign(pI->vertices->begin(),pI->vertices->end()); // copy vertices
      smap.captions=*pI->captions; // copy captions
      smap.m_colors.resize(pI->vertices->size());   //
      for (uint i=0;i<smap.m_colors.size();i+=3) {
          smap.m_colors[i]=1.0f;
          smap.m_colors[i+1]=0.0f;
          smap.m_colors[i+2]=0.0f;
      }
      //// заглавный шарик рисуем синим TODO цвет лучше задавать в плагине
      /// не отрисовывает так, потомучто перекрывается в этой позиции последующим шариком с любым более старшим индексом
      smap.m_colors[1]=0.0f;
      smap.m_colors[1+1]=0.0f;
      smap.m_colors[1+2]=1.0f;

      //mbuilder->newmapbuild=false; // забрали карту, готовьте новую
      //mbuilder->m_mutex.unlock();

      // данные для выборки объектов предоставляет плагин
      smap.IDv.resize(smap.captions.size()*4);
      float index=QUANTIZEROFSELECTION;                    // index должен соответствовать коэфициенту в функции setSelected()
      for (int i=0;i<smap.captions.size();i++) {
          smap.IDv[i*4]=smap.IDm; // ID model
          smap.IDv[i*4+1]=i*index; //((float)i/(float)karta->captions.size());    // ID point ДЕЛЁННОЕ НА КОЛИЧЕСТВО ФРАГМЕНТОВ
          smap.IDv[i*4+2]=0;    // reserv
          smap.IDv[i*4+3]=1.0f;  //
          /// СТАВЯТСЯ В СООТВЕТСТВИЕ ИНДЕКСЫ ВЫБОРКИ И ИНДЕКСЫ ВЫБОРА ПЛАГИНА idVPlugin=IDv=iCaptions
      }
  }//*/
}
