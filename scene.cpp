#include "scene.h"

#include <QMatrix4x4>

Scene::Scene(QWidget *parent) :
    QOpenGLWidget (parent),
    step(0.005f), // шаг сдвига фигур
    angle_x(0),
    angle_y(0),
    angle_z(0),
    perspective(true),
    paintMode(TEST_MODE)
{
  // задаём для виджета фокус, чтобы оно реагировало на кнопки
  this->setFocusPolicy(Qt::StrongFocus);
  // подключаем таймер к слоту
  connect(&m_timer,SIGNAL(timeout()),this,SLOT(slotAnimation()));
  // запускаем таймер
  m_timer.start(20);

  /*/ Specify the format and create platform-specific surface // В ATI HD 4000 ломает рисовку
  QSurfaceFormat format;
  format.setDepthBufferSize( 24 );
  format.setSamples( 4 );
  format.setStencilBufferSize(8);
  //format.setVersion(2,0);
  //format.setProfile( QSurfaceFormat::CompatibilityProfile );//( QSurfaceFormat::NoProfile ); // NoProfile for OGL<3.2 ( QSurfaceFormat::CoreProfile ); //// ( QSurfaceFormat::CompatibilityProfile )
  setFormat( format );//*/

  //  параметры камеры устанавливаются в initializeGL

  mbuilder=new MapBuilder();
  selectmode=false;
}

Scene::~Scene()
{
makeCurrent();      // тут используется текущий контекст системы, т.к. QOpenGLWidget::currentContext() уже нет
delete m_triangle;
    delete spherepoints;
delete m_shphere;
delete m_text;
delete karta;
doneCurrent();
//delete mbuilder;
}

void Scene::reset(){
    step=0.005f; // шаг сдвига фигур
    angle_x=0;
    angle_y=0;
    angle_z=0;
    perspective=true;
    setFigureInfo(); //
    //  параметры камеры
    camera.reset();// сброс камеры
    cameraFocusAngle=40;                // устанавливаем начальный угол проекции
    camera.pos=QVector3D(0.0f,0.0f,1.26f);
    camera.setView(QVector3D(0.0f,0.0f,0.0f));
    mouse_sensitivity=1.0f;
    setCamera(); // устанавливаем параметры камеры
}

void Scene::initializeGL() {
       f = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_3_0>();
                if ( !f ) {
                    qWarning( "Could not obtain OpenGL versions object&quot" );
                    exit( 1 );
                }
            f->initializeOpenGLFunctions();

    //
    qDebug() << QString((const char*)f->glGetString(GL_VERSION)) << "\n" <<
                QString((const char*)f->glGetString(GL_VENDOR))<< "\n" <<
                QString((const char*)f->glGetString(GL_RENDERER));//<< "\n" << glGetString(GL_EXTENTIONS);

    qDebug() << "Version format= " << QOpenGLContext::currentContext()->format().majorVersion() <<
                " " << QOpenGLContext::currentContext()->format().minorVersion();

    // очищаем поле
    //glClearColor(0.1f,0.1f,0.2f,1.0f); // тёмно-синенький
    f->glClearColor(0.1f,0.1f,0.2f,1.0f); //

    //создаём объект - треугольник (создаём один, а нарисовать можем много таких-же)
    //m_triangle=new Triangle(&m_program, m_vertexAttr, m_colorAttr, m_texAttr, m_texUniform);
    m_triangle=new Triangle();

    // инициализируем массив координат для сетевой сферы
    spherepoints=new pointsofsphere(300);

    // тест - создаём простую сферу на шейдерах
    m_shphere=new shphere();
    if (perspective)                // инициализируем зависимости от перспективы
        m_shphere->setPerspective();
            else
    m_shphere->setOrthogonal();

    // тект
    m_text =new Text();

    // устанавливаем параметры сцены
    near_=0.00001f;
    far_=100.0f;
    range=far_-near_;

    // устанавливаем параметры камеры
    reset();
    setCamera();
    setFigureInfo(); //

    // karta
    karta = new Karta();
    karta->IDm = 0.5f; //((float)1 / (float)255)*128; // устанавливаем ID модели

    // инициализируем свой буфер выбора FBO
    //QOpenGLFramebufferObjectFormat fboFormat;
    //fboFormat.setSamples(4);
    //fboFormat.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
    //fbo= new QOpenGLFramebufferObject(viewport.z*viewport.w*4);
}

void Scene::paintGL(){
    switch (paintMode){ // 1- devmode 2- prommode
      case 1:
        paintDM();      // test
        break;
      case 2:
        setStates();     // включаем буфер глубины, свет и прочее (возможно можно вынести в initGL)
        if (selectmode) {
            // инициализируем свой буфер выбора FBO
            QOpenGLFramebufferObjectFormat fboFormat;
            //fboFormat.setSamples(16); // нельзя будет иcпользовать glReadPixels
            fboFormat.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);  // нужно чтобы формирование текстуры было также с учётом теста глубины и прозрачности, как и буфер рисования основной сцены, это влияет на правильность выбора объектов при совпадении вершин
            fboFormat.setInternalTextureFormat(GL_RGBA32F);
            QOpenGLFramebufferObject fbo(viewport.z(), viewport.w(), fboFormat);

            // TODO pushGLstate()
            glDisable(GL_LIGHTING);
            glDisable(GL_COLOR_MATERIAL);
            glClearColor(0.0f,0.0f,0.0f,0.0f); //
            fbo.bind();     // рисуем в текстуру

            /*GLuint texture_map;
            glGenTextures(1, &texture_map);
            glBindTexture(GL_TEXTURE_2D, texture_map);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F , viewport.z(), viewport.w(), 0, GL_RGBA, GL_FLOAT, NULL);
            glBindTexture(GL_TEXTURE_2D, 0);
            // Build the framebuffer.
            GLuint framebuffer;
            f->glGenFramebuffers(1, &framebuffer);
            f->glBindFramebuffer(GL_FRAMEBUFFER, (GLuint)framebuffer);
            f->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_map, 0);
            GLenum status = f->glCheckFramebufferStatus(GL_FRAMEBUFFER);
            if (status != GL_FRAMEBUFFER_COMPLETE)
                qDebug() <<  "Error selectFBO" ;//*/

            paintKarta();
            // TODO popGLstate()
            glEnable(GL_LIGHTING);
            glEnable(GL_COLOR_MATERIAL);
            glClearColor(0.1f,0.1f,0.2f,1.0f); //

            glPixelStorei(GL_PACK_ALIGNMENT, 1);
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
            GLfloat pix[]={0.0,0.0,0.0,0.0};
            //GLubyte pix[4];
            //glReadPixels ( pmouse.x(), viewport.w()-pmouse.y(), 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &pix );
            glReadPixels ( pmouse.x(), viewport.w()-pmouse.y(), 1, 1, GL_RGBA, GL_FLOAT, &pix );
            selectID.setX(pix[0]); selectID.setY(pix[1]); selectID.setZ(pix[2]);selectID.setW(pix[3]);
            qDebug() << "x - " << pmouse.x() << " y - " << viewport.w()-pmouse.y();
            qDebug() << " dr-" << pix[0] << ", current_center.z-" << pix[1] << ", gl_FragCoord.z-" << pix[2] << ", pix[3]-" << pix[3];

            /*// release selectFBO
            f->glBindFramebuffer(GL_FRAMEBUFFER, 0);
            f->glDeleteTextures(1,&texture_map);
            f->glDeleteFramebuffers(1,&framebuffer);//*/

            /*QImage image(fbo.toImage());
            image.save("D:\\image.png","PNG");
            QRgb pixel=image.pixel(pmouse.x(),pmouse.y());
            qDebug() << "R - " << qRed(pixel) << "G - " << qGreen(pixel) << "B - " << qBlue(pixel) << "A - " << qAlpha(pixel);
            //*/
            fbo.release();

            selectmode=false;
            setSelected();
        }
        paintKarta();   //
        break;
      default:
        break;
      }
}

void Scene::setSelected(){
  if (!selectID.w()) return;
    float index=0.001f;                    // index должен соответствовать в функции назначения идентификаторов
    float fi=selectID.y()/index;
    int i= (fi<0) ? (int)ceil(fi) : (int)floor(fi);
    float foi=oldSelectID.y()/index;
    int oi= (foi<0) ? (int)ceil(foi) : (int)floor(foi);
    karta->m_colors[oi*3]=1.0f;
    karta->m_colors[oi*3+1]=0.0f;
    karta->m_colors[oi*3+2]=0.0f;
    karta->m_colors[i*3]=0.0f;
    karta->m_colors[i*3+1]=0.0f;
    karta->m_colors[i*3+2]=1.0f;
    //qDebug() << "index i=" << i << " fi" << fi << "oi" << oi;
    oldSelectID=selectID;
}

void Scene::paintDM()
{
  setStates();                    // включаем буфер глубины, свет и прочее (возможно можно вынести в initGL)

  //glClear(GL_COLOR_BUFFER_BIT); // если включен буфер глубины, то без его очистки мы ничего не увидим
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  //glMatrixMode(GL_PROJECTION);
  //qgluPerspective(60.0, width / height, 0.01, 15.0);

  //glMatrixMode(GL_MODELVIEW);

  //инициализируем матрицы преобразований
  PM.setToIdentity();
  if (perspective) {
      // устанавливаем трёхмерную канву (в перспективной проекции) для рисования (плоскости отсечения)
      // угол перспективы, отношение сторон, расстояние до ближней отсекающей плоскости и дальней
      PM.perspective(cameraFocusAngle,ratio,near_,far_);  // glFrustum( xmin, xmax, ymin, ymax, near, far)  // gluPerspective(fovy, aspect, near, far)
  }
  else {
      // устанавливаем трёхмерную канву (в ортогональной проекции) для рисования (плоскости отсечения)
      PM.ortho(-viewport.z()/viewport.w()*2,viewport.z()/viewport.w()*2,-2.0f,2.0f,-8.0f,8.0f); // glOrtho(left,right,bottom,top,near,far) // увеличение значений уменьшает фигуры на сцене (по Z задаём больше, чтобы не видеть отсечение фигур)
      // переносим по z дальше, обязательное условие для перспективной проекции // по оси z 0 это "глаз", - движение камеры назад, + вперёд.
  }
  MVPM=PM*MVM;

  // находим проекционную инверсную мтрицу
  bool inverted;
  PMi=PM.inverted(&inverted);
  if (!inverted)
      qDebug() << "PMi не конвертится";
  MVPMi=MVPM.inverted(&inverted);
  if (!inverted)
      qDebug() << "MVPMi не конвертится";

  /*/ находим обратную матрицу
  QMatrix4x4 VInverse ( // TODO: move this one to CPU // лишний код который вычисляется на раз в сцене (не надо его считать для каждой вершины)
          2.0f/viewport.z(), 0.0f, 0.0f, 0.0f,
          0.0f, 2.0f/viewport.w(), 0.0f, 0.0f,
          0.0f, 0.0f, 2.0f/(far_-near_), 0.0f,
          -(viewport.z()+2.0f*viewport.x())/viewport.z(), -(viewport.w()+2.0f*viewport.y())/viewport.w(), -(near_+far_)/(far_-near_), 1.0f);
  QMatrix4x4 VPInverse = PMi*VInverse;//*/

  // РИСУЕМ ТРЕУГОЛЬНИК
  // инициализируем данные программы матрицы и атрибуты
  m_triangle->init();
  // зaпихиваем в его программу матрицу ориентации
  m_triangle->m_program.setUniformValue(m_triangle->m_matrixUniform, MVPM);
  // вызываем функцию рисования объекта (или объектов в for)
  m_triangle->draw();
  // проводим сброс инициализации параметров
  m_triangle->drop();//*/

  //РИСУЕМ СФЕРЫ
  m_shphere->init();
  m_shphere->m_program->setUniformValue(m_shphere->m_matrixUniform, MVPM);
  m_shphere->m_program->setUniformValue("PM", PM);                          // TODO вынести в класс шфер
  //m_shphere->m_program->setUniformValue("near", near_);                          // TODO вынести в класс шфер
  //m_shphere->m_program->setUniformValue("far", far_);                          // TODO вынести в класс шфер
  m_shphere->m_program->setUniformValue("range", range);                          // TODO вынести в класс шфер
  m_shphere->m_program->setUniformValue("PMi", PMi);                          // TODO вынести в класс шфер
    //m_shphere->m_program->setUniformValue("VPInverse", VPInverse);                          // TODO вынести в класс шфер
    //m_shphere->m_program->setUniformValue("VInverse", VInverse);                          // TODO вынести в класс шфер
  m_shphere->m_program->setUniformValue("MVM", MVM);
  m_shphere->m_program->setUniformValue("MVPMi", MVPMi);
  m_shphere->m_program->setUniformValue("viewport",viewport);
  m_shphere->draw();
  m_shphere->drop();//*/

  //РИСУЕМ ТЕКСТ
  //m_text->font=QFont::Bold;
  //m_text->pen=
  m_text->set();
  m_text->drawO(MVPM,"Точка 1",QVector3D(m_triangle->m_vertices[0],m_triangle->m_vertices[1],m_triangle->m_vertices[2]));
  m_text->drawO(MVPM,"Точка 2",QVector3D(m_triangle->m_vertices[3],m_triangle->m_vertices[4],m_triangle->m_vertices[5]));
  m_text->drawO(MVPM,"Точка 3",QVector3D(m_triangle->m_vertices[6],m_triangle->m_vertices[7],m_triangle->m_vertices[8]));
  m_text->reset();
}

void Scene::paintKarta()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    PM.setToIdentity();
    PM.perspective(cameraFocusAngle,ratio,near_,far_);  // glFrustum( xmin, xmax, ymin, ymax, near, far)  // gluPerspective(fovy, aspect, near, far)
    //MVM.rotate(angle_z,0.0f,1.0f,0.0f);  // поворот вокруг оси центра координат
    //MVM=CameraView; //*MVM;  // получаем матрицу трансформации итогового вида
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

void Scene::paintFlatMap()
{
  //проверяем, готовы ли данные, если да, то копируем для отображения
  if (mbuilder->newmapbuild)  { /// TODO Перевести в сигнал  (и в модуль карты)
      mbuilder->m_mutex.lock();
      karta->vertices.assign(mbuilder->vertices.begin(),mbuilder->vertices.end()); // copy vertices
      karta->captions=mbuilder->captions; // copy captions
      karta->m_colors.resize(mbuilder->vertices.size());   //
      for (uint i=0;i<karta->m_colors.size();i+=3) {
          karta->m_colors[i]=1.0f;
          karta->m_colors[i+1]=0.0f;
          karta->m_colors[i+2]=0.0f;
      }
      mbuilder->newmapbuild=false; // забрали карту, готовьте новую
      mbuilder->m_mutex.unlock();

      // готовим данные для выборки объектов (TODO убрать ограничение в 255 объектов)
      karta->IDv.resize(karta->captions.size()*4);
      float index=0.001f;                    // index должен соответствовать функции setSelected()
      for (int i=0;i<karta->captions.size();i++) {
          karta->IDv[i*4]=karta->IDm; // ID model
          karta->IDv[i*4+1]=i*index; //((float)i/(float)karta->captions.size());    // ID point ДЕЛЁННОЕ НА КОЛИЧЕСТВО ФРАГМЕНТОВ
          karta->IDv[i*4+2]=0;    // reserv
          karta->IDv[i*4+3]=1.0f;  //
      }
    }

  //karta->draw();

  // РИСУЕМ ЛИНИИ
    //подключаем программу и проверяем
    if (!karta->program.bind()){
        qWarning("KARTA. Line. Шейдеры не сбиндились");
        return;
    }
    // устанавливаем место хранения координат
      karta->program.setAttributeArray(karta->m_vertexAttr, karta->vertices.data(), 3);
      // активируем массивы
      karta->program.enableAttributeArray(karta->m_vertexAttr);
      // зaпихиваем в его программу матрицу ориентации
      karta->program.setUniformValue(karta->m_MVPmatrix, MVPM);
      // устанавливаем цвет линий
      karta->program.setUniformValue("colormode", true);    // рисуем одним цветом, не массивом
      karta->program.setUniformValue("statcolor", QVector4D(0.0f,1.0f,0.0f,1.0f));  // c прозрачным цветом (0,0f) на ATI не рисуется
      //glLineWidth(10);
      // рисуем линии
      glDrawArrays(GL_LINES,0,karta->vertices.size()/3);
      // деактивируем массивы
      karta->program.disableAttributeArray(karta->m_vertexAttr);
      // очищаем программу
      karta->program.release();

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
      m_shphere->m_program->setAttributeArray(m_shphere->m_vertexAttr, karta->vertices.data(), 3);
      m_shphere->m_program->setAttributeArray(m_shphere->m_colorAttr, karta->m_colors.data(), 3);
      m_shphere->m_program->setUniformValue("R",m_shphere->radius);
      m_shphere->m_program->setUniformValue("maxpointsize",m_shphere->maxpointsize);
      // активируем массивы
      m_shphere->m_program->enableAttributeArray(m_shphere->m_vertexAttr);
      m_shphere->m_program->enableAttributeArray(m_shphere->m_colorAttr);
      // активируем выборку объектов
      m_shphere->m_program->setAttributeArray("selectID", karta->IDv.data(), 4);
      m_shphere->m_program->enableAttributeArray("selectID");
      m_shphere->m_program->setUniformValue("selectmode", selectmode);    // указываем режим выборки
      // рисуем точки
      glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);   // говорим что будут меняться размеры точек в шейдере
      glDrawArrays(GL_POINTS,0,karta->vertices.size()/3);
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
      for (int i=0;i<karta->captions.size();i++) {
          m_text->drawO(MVPM,karta->captions[i],QVector3D(karta->vertices[i*3],karta->vertices[(i*3)+1],karta->vertices[(i*3)+2]));
      }
      m_text->reset();
}

void Scene::resizeGL(int w, int h){
    ratio = (1.0*w)/(!h?1:h);   // !h?1:h - защита от деления на 0
    glViewport(0,0,w,h);
    viewport.setX(0); viewport.setY(0); viewport.setZ((float)w); viewport.setW((float)h);
}

void Scene::setStates()
{
    // glEnable(GL_BRAINS) // ;)

    // glEnable(GL_BLEND); // На старых картах ATI вызывает проблемы в шейдере (цветовая глубина в профиле для блендинга должна быть 32 т.е. с альфой)

    glEnable(GL_DEPTH_TEST);  // без этого у нас не будет эффекта глубины (включаем буфер глубины)
    //glEnable(GL_CULL_FACE); // активирует устранение спрятанных поверхностей.
    glEnable(GL_LIGHTING);
    glEnable(GL_COLOR_MATERIAL);  // было отключено
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_NORMALIZE);

    setLights();
}

void Scene::setLights()
{
    //float lightColour[] = {1.0f, 0.9f, 0.9f, 1.0f};
    ///glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColour);

    float lightDir[] = {1.0f, 1.0f, 1.0f, 0.0f};
    //glLightfv(GL_LIGHT0, GL_SPECULAR, lightColour);
    glLightfv(GL_LIGHT0, GL_POSITION, lightDir);

    glLightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER, 1.0f);
    glEnable(GL_LIGHT0);
}

void Scene::defaultStates()
{
    //glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glDisable(GL_LIGHTING);
    //glDisable(GL_COLOR_MATERIAL);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_LIGHT0);
    glDisable(GL_NORMALIZE);

    glLightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER, 0.0f);
    float defaultMaterialSpecular[] = {0.0f, 0.0f, 0.0f, 1.0f};
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, defaultMaterialSpecular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 0.0f);
}

void Scene::setPerspective(int _switch)
{
    if (_switch==0) perspective=true;
    else perspective=false;
}

void Scene::setPaintMode(int mode)
{
    paintMode=mode; // 1- test, 2 - karta
}

void Scene::buildNewMap()
{
    mbuilder->newmap(true);   //false - добавка к существующей, true - построение новой с нуля
}

void Scene::addMap()
{
    mbuilder->newmap(false);   //false - добавка к существующей, true - построение новой с нуля
}

void Scene::keyPressEvent(QKeyEvent *event)
{
  if (event->modifiers() & Qt::ShiftModifier) {
      camera.strated=true;  // режим парящей камеры
      emit setBar(QString("strated=true"));
    }
  else {
      camera.strated=false;
      emit setBar(QString("strated=false"));
    }
  switch (event->key()) {
    case Qt::Key_Up:
      camera.moveUD(step);
      break;
    case Qt::Key_Down:
      camera.moveUD(-step);
      break;
    case Qt::Key_Left:
      camera.moveLR(-step);
      break;
    case Qt::Key_Right:
      camera.moveLR(step);
      break;
  case Qt::Key_W:
      --angle_x;
      if (angle_x<0) angle_x=359;
      //camera.moveUD(+step);
      camera.Rotate_PositionX(+1.0f); // поворот по оси х
    break;
  case Qt::Key_S:
      ++angle_x;
      if (angle_x>=360) angle_x=0;
      //camera.moveUD(-step);
      camera.Rotate_PositionX(-1.0f); // поворот по оси х
    break;
  case Qt::Key_A:
      --angle_y;
      if (angle_y<0) angle_y=359;
      camera.Rotate_PositionY(-1.0f); // поворот по оси y
    break;
  case Qt::Key_D:
      ++angle_y;
      if (angle_y>=360) angle_y=0;
      camera.Rotate_PositionY(+1.0f); // поворот по оси y
    break;
  case Qt::Key_Q:
        --angle_z;
        if (angle_z<0) angle_z=359;
        //camera.turnOZ(-1.0f);
        camera.twirl(-1.0f);
    break;
  case Qt::Key_E:
        ++angle_z;
        if (angle_z>=360) angle_z=0;
        //camera.turnOZ(+1.0f);
        camera.twirl(+1.0f);
    break;
    case Qt::Key_T:
        camera.turnUD(+1.0f);
      break;
    case Qt::Key_G:
        camera.turnUD(-1.0f);
      break;
    case Qt::Key_F:
        camera.turnLR(+1.0f);
      break;
    case Qt::Key_H:
        camera.turnLR(-1.0f);
      break;
    case Qt::Key_M:
        m_shphere->radius=m_shphere->radius+0.001;
        m_triangle->setx0(m_triangle->m_x0-0.001); // +
        m_triangle->setsize(m_triangle->m_size+0.004); // +
        m_shphere->setx0(m_shphere->m_x0-0.001);
        m_shphere->setdist(m_shphere->m_dist+0.004);
      break;
    case Qt::Key_N:
      if (m_shphere->radius>=0.001f) {
          m_shphere->radius=m_shphere->radius-0.001f;
          m_triangle->setx0(m_triangle->m_x0+0.001); // -
          m_triangle->setsize(m_triangle->m_size-0.004); // -
          m_shphere->setx0(m_shphere->m_x0+0.001);
          m_shphere->setdist(m_shphere->m_dist-0.004);
        }
      break;
  case Qt::Key_I: // уменьшаем угол перспективы камеры min=10 // TODO ещё по идее надо отодвигать камеру, чтобы пирамида охватывала сцену
      if (cameraFocusAngle>10) --cameraFocusAngle;
    break;
  case Qt::Key_O: // увеличиваем угол перспективы камеры max=120 // TODO приближать камеру
      if (cameraFocusAngle<120) ++cameraFocusAngle;
    break;
  case Qt::Key_R:
      reset();
    break;
    default:
      break;
    }
  setCamera();
  setFigureInfo();
  update();
}

// ArcBall Rotation
// http://pmg.org.ru/nehe/nehe48.htm
// масштабируем, координаты мыши из диапазона [0…ширина], [0...высота] в диапазон [-1...1], [1...-1]
// (запомните, что мы меняем знак координаты Y, чтобы получить корректный результат в OpenGL)
QPointF Scene::pixelPosToViewPos(const QPointF& p)
{
    return QPointF(2.0 * float(p.x()) / width() - 1.0,
                   1.0 - 2.0 * float(p.y()) / height());
}

void Scene::mouseDoubleClickEvent(QMouseEvent *event)
{
    pmouse=event->localPos();
    selectmode=true;
}

void Scene::mousePressEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
       camera.push(pixelPosToViewPos(event->localPos()));
    }
}

void Scene::mouseReleaseEvent(QMouseEvent *event)
{
  if (event) step=step;
}

void Scene::mouseMoveEvent(QMouseEvent *event)
{
  if (event->buttons() & Qt::LeftButton) {
     camera.moveByMouse(pixelPosToViewPos(event->localPos()));
     setCamera();
  }
}

void Scene::wheelEvent(QWheelEvent *event)
{
    if( event->modifiers() & Qt::ShiftModifier )
    {
        //camera.strated=true;  // режим парящей камеры
        //emit setBar(QString("strated=true"));
    }
    else {
        //camera.strated=false;  // режим парящей камеры
        //emit setBar(QString("strated=false"));
    }
    if( event->modifiers() & Qt::ControlModifier )
    {
        // do something even awesomer!
    }
// шаг колеса обычно 120 едениц, 1 еденица это 1/8 градуса, значит 1 шаг = 15 градусам.
// мы будем считать в еденицах (некоторые драйвера мыши дают точность больше, т.е. меньше 120 за такт)
// move to new position by step 120/10000 пока только по оси Z (-delta - значит крутим на себя)
camera.moveFB((float)event->angleDelta().y()/10000);
setCamera();
}

void Scene::setCamera() {
    camera.apply();
    MVM=camera.CameraView;
    setCameraInfo();
}

void Scene::setCameraInfo()
{
    QString text="I,O - изменение проекции. Угол проекции="+QString().setNum(cameraFocusAngle);
    text=text+" cameraEye="+QString().setNum(camera.pos[0])+", "+QString().setNum(camera.pos[1])+", "+QString().setNum(camera.pos[2])
            +", cameraCenter="+QString().setNum(camera.posView[0])+", "+QString().setNum(camera.posView[1])+", "+QString().setNum(camera.posView[2])
            +" cameraUp="+QString().setNum(camera.camUp[0])+", "+QString().setNum(camera.camUp[1])+", "+QString().setNum(camera.camUp[2]);
    emit setPerspectiveInfo(text);
}

void Scene::setFigureInfo()
{
    QString text=m_triangle->getFigureInfo()+", "+m_shphere->getFigureInfo()+", Угол поворота -Z="+QString().setNum(angle_z)+"°";
    emit setFiguresInfo(text);
    text="M, N - изменение фигуры. Radius="+QString().setNum(m_shphere->radius);
    emit setFiguresInfo2(text);
}

void Scene::slotAnimation()
{
  ///++angle_z; // поворот на 1 градус?
  ///if (angle_z>=360) angle_z=0;
  //m_triangle->setx0(m_triangle->m_x0-step);
  update();
}
