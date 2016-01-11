#include "scene.h"

#include <QMatrix4x4>
#include <QAction>

Scene::Scene(QWidget *parent) :
    QOpenGLWidget (parent),
    karta(nullptr),
    step(0.005f), // шаг сдвига фигур
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
for (uint i=0;i<levels.size();i++) {
    delete levels[i];
  }
doneCurrent();
//delete mbuilder;
}

void Scene::reset(){
    step=0.005f; // шаг сдвига фигур
    angle_z=0;
    perspective=true;
    setFigureInfo(); //
    //  параметры камеры
    currentLevel->reset();  // init level, init camera
    mouse_sensitivity=1.0f;
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

    // karta
    karta = new Karta();
    karta->smap.IDm = 0.5f; //((float)1 / (float)255)*128; // устанавливаем ID модели
    /// подключаем интерфейс получения данных от плагина-картостроителя (id "BuilderMapInterface")
    connect(this,SIGNAL(buildFromPlugin(QAction*)),karta,SLOT(buildFromPlugin(QAction*)));

    // инициализируем свой буфер выбора FBO
    //QOpenGLFramebufferObjectFormat fboFormat;
    //fboFormat.setSamples(4);
    //fboFormat.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
    //fbo= new QOpenGLFramebufferObject(viewport.z*viewport.w*4);

    // levels (load and initialize)
    currentLevel = new Level();
    /// отображение информации со сцены
    connect(currentLevel,SIGNAL(setCameraInfo(QString)),this,SIGNAL(setCameraInfo(QString)));
    // заносим уровень в список
    { int size; size=levels.size(); levels.resize(size+1); levels[size]=currentLevel;}
    karta->level=currentLevel;

    setFigureInfo(); //
}

void Scene::paintGL(){
    switch (paintMode){
      case TEST_MODE:
        paintDM();      // test
        break;
      case KARTA_MODE:
        setStates();     // включаем буфер глубины, свет и прочее (возможно можно вынести в initGL)
        if (selectmode) {
            // инициализируем свой буфер выбора FBO
            QOpenGLFramebufferObjectFormat fboFormat;
            //fboFormat.setSamples(16); // нельзя будет иcпользовать glReadPixels
            fboFormat.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);  // нужно чтобы формирование текстуры было также с учётом теста глубины и прозрачности, как и буфер рисования основной сцены, это влияет на правильность выбора объектов при совпадении вершин
            fboFormat.setInternalTextureFormat(GL_RGBA32F);
            QOpenGLFramebufferObject fbo(currentLevel->viewport.z(), currentLevel->viewport.w(), fboFormat);

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

            paintKarta(DRAW_SFBO);
            // TODO popGLstate()
            glEnable(GL_LIGHTING);
            glEnable(GL_COLOR_MATERIAL);
            glClearColor(0.1f,0.1f,0.2f,1.0f); //

            //glPixelStorei(GL_PACK_ALIGNMENT, 1);
            //glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
            GLfloat pix[]={0.0,0.0,0.0,0.0};
            //GLubyte pix[4];
            //glReadPixels ( pmouse.x(), viewport.w()-pmouse.y(), 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &pix );
            glReadPixels ( pmouse.x(), currentLevel->viewport.w()-pmouse.y(), 1, 1, GL_RGBA, GL_FLOAT, &pix );
            selectID.setX(pix[0]); selectID.setY(pix[1]); selectID.setZ(pix[2]);selectID.setW(pix[3]);
            qDebug() << "x - " << pmouse.x() << " y - " << currentLevel->viewport.w()-pmouse.y();
            qDebug() << "pix[0]" << pix[0] << ", pix[1]-" << pix[1] << ", pix[2]-" << pix[2] << ", pix[3]-" << pix[3];

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
        paintKarta(DRAW_NORM);
        break;
      default:
        break;
      }
}

void Scene::setSelected(){
  if (!selectID.w()) return;
    float index=QUANTIZEROFSELECTION;  // index должен соответствовать в функции назначения идентификаторов
    //float fi=selectID.y()/index;
    //int i= (fi<0) ? (int)ceil(fi) : (int)floor(fi); ///TODO, это неверное определение индекса, (всегда работает floor) повезло что карта округляет(корректирует) цвет в сторону увеличения (округляет (int))
    //float foi=oldSelectID.y()/index;
    //int oi= (foi<0) ? (int)ceil(foi) : (int)floor(foi);
    int i=round(selectID.y()/index);    //округление к ближайшему целому
    int oi=round(oldSelectID.y()/index);
    karta->smap.m_colors[oi*3]=1.0f;
    karta->smap.m_colors[oi*3+1]=0.0f;
    karta->smap.m_colors[oi*3+2]=0.0f;
    karta->smap.m_colors[i*3]=0.0f;
    karta->smap.m_colors[i*3+1]=0.0f;
    karta->smap.m_colors[i*3+2]=1.0f;
    //qDebug() << "index i=" << i << " fi" << fi << "oi" << oi;
    oldSelectID=selectID;
    /// запускаем стройку новой карты в соответствии с выбором
    if (karta->plugin_data) {
        karta->mapFromPlugin(i);
      }
}

void Scene::paintKarta(int draw_law) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    currentLevel->PM.setToIdentity();
    currentLevel->PM.perspective(currentLevel->cameraFocusAngle,currentLevel->ratio,currentLevel->near_,currentLevel->far_);  // glFrustum( xmin, xmax, ymin, ymax, near, far)  // gluPerspective(fovy, aspect, near, far)
    currentLevel->MVPM=currentLevel->PM*currentLevel->MVM;

    // находим проекционную инверсную мтрицу
    bool inverted;
    currentLevel->PMi=currentLevel->PM.inverted(&inverted);
    if (!inverted)
        qDebug() << "PMi не конвертится";
    currentLevel->MVPMi=currentLevel->MVPM.inverted(&inverted);
    if (!inverted)
        qDebug() << "MVPMi не конвертится";

    if (karta->mbuilder->currentmap==FLAT_MAP) karta->paintFlatMap(draw_law);
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
  currentLevel->PM.setToIdentity();
  if (perspective) {
      // устанавливаем трёхмерную канву (в перспективной проекции) для рисования (плоскости отсечения)
      // угол перспективы, отношение сторон, расстояние до ближней отсекающей плоскости и дальней
      currentLevel->PM.perspective(currentLevel->cameraFocusAngle,currentLevel->ratio,currentLevel->near_,currentLevel->far_);  // glFrustum( xmin, xmax, ymin, ymax, near, far)  // gluPerspective(fovy, aspect, near, far)
  }
  else {
      // устанавливаем трёхмерную канву (в ортогональной проекции) для рисования (плоскости отсечения)
      currentLevel->PM.ortho(-currentLevel->viewport.z() / currentLevel->viewport.w()*2, currentLevel->viewport.z() / currentLevel->viewport.w()*2,-2.0f,2.0f,-8.0f,8.0f); // glOrtho(left,right,bottom,top,near,far) // увеличение значений уменьшает фигуры на сцене (по Z задаём больше, чтобы не видеть отсечение фигур)
      // переносим по z дальше, обязательное условие для перспективной проекции // по оси z 0 это "глаз", - движение камеры назад, + вперёд.
  }
  currentLevel->MVPM=currentLevel->PM*currentLevel->MVM;

  // находим проекционную инверсную мтрицу
  bool inverted;
  currentLevel->PMi=currentLevel->PM.inverted(&inverted);
  if (!inverted)
      qDebug() << "PMi не конвертится";
  currentLevel->MVPMi=currentLevel->MVPM.inverted(&inverted);
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
  m_triangle->m_program.setUniformValue(m_triangle->m_matrixUniform, currentLevel->MVPM);
  // вызываем функцию рисования объекта (или объектов в for)
  m_triangle->draw();
  // проводим сброс инициализации параметров
  m_triangle->drop();//*/

  //РИСУЕМ СФЕРЫ
  m_shphere->init();
  m_shphere->m_program->setUniformValue(m_shphere->m_matrixUniform, currentLevel->MVPM);
  m_shphere->m_program->setUniformValue("PM", currentLevel->PM);                          // TODO вынести в класс шфер
  m_shphere->m_program->setUniformValue("range", currentLevel->range);                          // TODO вынести в класс шфер
  m_shphere->m_program->setUniformValue("PMi", currentLevel->PMi);                          // TODO вынести в класс шфер
    //m_shphere->m_program->setUniformValue("VPInverse", VPInverse);                          // TODO вынести в класс шфер
    //m_shphere->m_program->setUniformValue("VInverse", VInverse);                          // TODO вынести в класс шфер
  m_shphere->m_program->setUniformValue("MVM", currentLevel->MVM);
  m_shphere->m_program->setUniformValue("MVPMi", currentLevel->MVPMi);
  m_shphere->m_program->setUniformValue("viewport",currentLevel->viewport);
  m_shphere->draw();
  m_shphere->drop();//*/

  //РИСУЕМ ТЕКСТ
  //m_text->font=QFont::Bold;
  //m_text->pen=
  m_text->set();
  m_text->drawO(currentLevel->MVPM,"Точка 1",QVector3D(m_triangle->m_vertices[0],m_triangle->m_vertices[1],m_triangle->m_vertices[2]));
  m_text->drawO(currentLevel->MVPM,"Точка 2",QVector3D(m_triangle->m_vertices[3],m_triangle->m_vertices[4],m_triangle->m_vertices[5]));
  m_text->drawO(currentLevel->MVPM,"Точка 3",QVector3D(m_triangle->m_vertices[6],m_triangle->m_vertices[7],m_triangle->m_vertices[8]));
  m_text->reset();
}

void Scene::resizeGL(int w, int h){
  /// TODO меняем на всех уровнях
    currentLevel->ratio = (1.0*w)/(!h?1:h);   // !h?1:h - защита от деления на 0
    glViewport(0,0,w,h);
    currentLevel->viewport.setX(0); currentLevel->viewport.setY(0); currentLevel->viewport.setZ((float)w); currentLevel->viewport.setW((float)h);
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
    if (karta!=nullptr)
        karta->plugin_data=false; // используется встроенные функции источника данных
}

void Scene::buildNewMap()
{
    karta->mbuilder->newmap(true);   //false - добавка к существующей, true - построение новой с нуля
}

void Scene::addMap()
{
    karta->mbuilder->newmap(false);   //false - добавка к существующей, true - построение новой с нуля
}

void Scene::keyPressEvent(QKeyEvent *event)
{
  if (event->modifiers() & Qt::ShiftModifier) {
      currentLevel->camera.strato=true;  // режим парящей камеры
      emit setBar(QString("strated=true"));
    }
  else {
      currentLevel->camera.strato=false;
      emit setBar(QString("strated=false"));
    }
  switch (event->key()) {
    case Qt::Key_Up:
      currentLevel->camera.moveFB(step);
      break;
    case Qt::Key_Down:
      currentLevel->camera.moveFB(-step);
      break;
    case Qt::Key_Left:
      currentLevel->camera.moveLR(-step);
      break;
    case Qt::Key_Right:
      currentLevel->camera.moveLR(step);
      break;
  case Qt::Key_W:
      currentLevel->camera.Rotate_PositionX(+1.0f); // поворот по оси х
    break;
  case Qt::Key_S:
      currentLevel->camera.Rotate_PositionX(-1.0f); // поворот по оси х
    break;
  case Qt::Key_A:
      currentLevel->camera.Rotate_PositionY(-1.0f); // поворот по оси y
    break;
  case Qt::Key_D:
      currentLevel->camera.Rotate_PositionY(+1.0f); // поворот по оси y
    break;
  case Qt::Key_Q:
        --angle_z;
        if (angle_z<0) angle_z=359;
        //camera.turnOZ(-1.0f);
        currentLevel->camera.twirl(-1.0f);
    break;
  case Qt::Key_E:
        ++angle_z;
        if (angle_z>=360) angle_z=0;
        //camera.turnOZ(+1.0f);
        currentLevel->camera.twirl(+1.0f);
    break;
    case Qt::Key_T:
        currentLevel->camera.turnUD(+1.0f);
      break;
    case Qt::Key_G:
        currentLevel->camera.turnUD(-1.0f);
      break;
    case Qt::Key_F:
        currentLevel->camera.turnLR(+1.0f);
      break;
    case Qt::Key_H:
        currentLevel->camera.turnLR(-1.0f);
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
      if (currentLevel->cameraFocusAngle>10) --currentLevel->cameraFocusAngle;
    break;
  case Qt::Key_O: // увеличиваем угол перспективы камеры max=120 // TODO приближать камеру
      if (currentLevel->cameraFocusAngle<120) ++currentLevel->cameraFocusAngle;
    break;
  case Qt::Key_R:
      currentLevel->reset();
    break;
    default:
      break;
    }
  currentLevel->setCamera();
  setFigureInfo();
  update();   // перерисовка экрана
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
    if (event->button() & Qt::LeftButton) {
        selectmode=selectmode;
    }
}

void Scene::mousePressEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
       currentLevel->camera.push(pixelPosToViewPos(event->localPos()));
       preSelectMode=true;  // ждём возможно выборку, а не перемещение
    }
}

void Scene::mouseReleaseEvent(QMouseEvent *event)
{
  if (event->button() & Qt::LeftButton) {
      if (preSelectMode) {
          preSelectMode=false;
          pmouse=event->localPos();    // координаты выборки
          selectmode=true;
      }
  }
}

void Scene::mouseMoveEvent(QMouseEvent *event)
{
  if (event->buttons() & Qt::LeftButton) {
      preSelectMode=false;  // выборки не будет, - перемещение
     currentLevel->camera.moveByMouse(pixelPosToViewPos(event->localPos()));
     currentLevel->setCamera();
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
//camera.moveFB((float)event->angleDelta().y()/10000);
    currentLevel->camera.distanceUD((float)event->angleDelta().y()/10000);
    currentLevel->setCamera();
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

void Scene::buildMapFromPlugin()
{
  QAction *pact = qobject_cast<QAction*>(sender());
  emit buildFromPlugin(pact);
}

