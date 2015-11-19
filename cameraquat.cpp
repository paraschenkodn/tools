#include "cameraquat.h"
#include <QLineF>

CameraQuat::CameraQuat()
{
q.setVector(0.0f,0.0f,0.0f);
q.setScalar(1.0f);
pos.setX(0.0f);pos.setY(0.0f);pos.setZ(0.0f);
rq.setVector(0.0f,0.0f,0.0f);
rq.setScalar(1.0f);

pos=QVector3D(0.0f,0.0f,1.0f);      //
pView=QVector3D(0.0f,0.0f,-1.0f);   // ((вектор взгляда)смещение от позиции камеры) позиция взгляда = позиция камеры + смещение

m_angularVelocity=0;
m_axis = QVector3D(0, 1, 0);            // начальный вектор вращения вдоль оси Y
m_lastTime = QTime::currentTime();      // зафиксируем текущее время (для вычислений с угловой скоростью)
mouse_sensitivity=1.0f;
}

void CameraQuat::Rotate_Position(float angle, float x, float y, float z)
{
  // версия для клавиатуры
  // величина шага поворота (или разница между позициями мыши)
  float delta = angle/100;//0.01f; //QLineF delta(m_lastPos, p);
  // угловая скорость для анимации
  //m_angularVelocity = 180*delta.length() / (PI*msecs);
  // вектор вокруг которого разворачиваем
  m_axis = QVector3D(0.0f,1.0f,0.0f);//m_axis = QVector3D(-delta.dy(), delta.dx(), 0.0f).normalized();
  //*********************************************************************************
  //*** версия не учитывает поворот осей координат, крутит только вокруг истинной оси
  //*/ // получаем развернутый по текущему кватерниону угол разворота (он у нас единичный)
  //*/ QQuaternion nq;
  //*/ m_axis = nq.rotatedVector(m_axis); //m_axis = transformation.rotatedVector(m_axis);
  //*/ // получаем кватернион поворота позиции камеры
  //*/ rq = QQuaternion::fromAxisAndAngle(m_axis, 180 / PI * delta) * rq;//m_rotation = QQuaternion::fromAxisAndAngle(m_axis, 180 / PI * delta.length()) * m_rotation;
  //**********************************************************************************
  // получаем позицию взгляда - точку вокруг которой строится вращение
  //QVector3D pView=q.rotatedVector(QVector3D(0,0,-1))+camera.pos;
  // теперь нам нужна вертикаль (для вращения вокруг неё по оси Y)
  m_axis = q.rotatedVector(m_axis)+pos+QVector3D(0,0,-1); // переносим вектор поворота в позицию камеры + смещение
  /**/ // получаем развернутый по текущему кватерниону угол разворота
  /**/ //QQuaternion nq;
  /**/ //m_axis = nq.rotatedVector(m_axis); //m_axis = transformation.rotatedVector(m_axis);
  /**/ // получаем кватернион поворота позиции камеры
  /**/ rq = rq * QQuaternion::fromAxisAndAngle(m_axis, 180 / PI * delta);//m_rotation = QQuaternion::fromAxisAndAngle(m_axis, 180 / PI * delta.length()) * m_rotation;
  //*************************************************************************************
}

/// входящие параметры
/// 1 - текущая точка в которой щёлкнули кнопкой (пересчитанной из экранной в координаты контекста отображения)
/// 2 - новый кватернион или сопряженный текущей расчитанной позиции расчитанной из угловой скорости раннее
void CameraQuat::moveByMouse(const QPointF &p, const QQuaternion &transformation)
{
    int msecs;                                  // перенесли объявление повыше
    QTime currentTime = QTime::currentTime();   // и взятие текущего времени тоже
    msecs = m_lastTime.msecsTo(currentTime);    // дельта времени

    if (msecs <= 20)    // лишний раз зачем считать?
        return;

    QLineF delta(m_lastPos, p);
    m_angularVelocity = 180*delta.length() / (PI*msecs);
    //*********************************************************
    //*/ этот кусок почемуто не учитывает поворот координат по Z, при повороте на 180 мышь получается инвертированной
    //*/ тоже самое если использовать функции turnOX, turnOY, turnOZ
    //*/ m_axis = QVector3D(-delta.dy(), delta.dx(), 0.0f).normalized();
    //*/ m_axis = transformation.rotatedVector(m_axis);
    //*/ q = QQuaternion::fromAxisAndAngle(m_axis, 180 / PI * delta.length()) * q;
    //*********************************************************
    /**/    turnLR(180 / PI * delta.dx()); // * mouse_sensitivity
    /**/    turnUD(180 / PI * -delta.dy()); // * mouse_sensitivity
    //*********************************************************
    m_lastPos = p;              // запоминаем последнюю позицию мыши
    m_lastTime = currentTime;   // запоминаем последнюю позицию времени (для расчёта скорости вращения (пригодится))
}

void CameraQuat::push(const QPointF& p)
{
    //m_rotation = rotation();            // запоминаем текущее значение поворота в кватернионе (с учётом прошедшего времени)
    //m_pressed = true;                   // ВЗВОДИМ флаг нажатия клавиш мыши
    m_lastTime = QTime::currentTime();  // запоминаем время когда запомнили текущий поворот (здесь возникает лаг с currentTime так как прошло время машшинной обработки команд и currentTime в функции rotation() != m_lastTime)
    m_lastPos = p;                      // запоминаем текущую точку в которой щёлкнули кнопкой (пересчитанной из экранной в координаты (сцены??))
    m_angularVelocity = 0.0f;           // устанавливаем угловую скорость равной 0 (тормозим вращение)
}
