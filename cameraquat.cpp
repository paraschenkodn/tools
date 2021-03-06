#include "cameraquat.h"
#include <QLineF>

CameraQuat::CameraQuat()
{
reset();  // установка начальных параметров
}

void CameraQuat::reset()   // установка начальных параметров
{
  q.setVector(0.0f,0.0f,0.0f);
  q.setScalar(1.0f);
  rq.setVector(0.0f,0.0f,0.0f);
  rq.setScalar(1.0f);

  pos=QVector3D(0.0f,0.0f,0.5f);      //
  pView=QVector3D(0.0f,0.0f,-0.5f);   // ((вектор взгляда)смещение от позиции камеры) позиция взгляда = позиция камеры + смещение
  posView=pos+pView;
  camUp=QVector3D(0.0f,1.0f,0.0f);  // нормализованный вектор верха

  q_angularVelocity=0;
  rq_angularVelocity=0;
  q_axis = QVector3D(0, 1, 0);            // начальный вектор вращения вдоль оси Y
  rq_axis = QVector3D(0, 1, 0);            // начальный вектор вращения вдоль оси Y
  m_lastTime = QTime::currentTime();      // зафиксируем текущее время (для вычислений с угловой скоростью)
  mouse_sensitivity=1.0f;

  grounded=true; // по умолчанию используем режим "заземления" (это значит что ориентируем камеру по горизонту, без завала при вращении)
  strato=false;  // режим "парение" по умолчанию выключен
}

// поворот от третьего лица X
void CameraQuat::Rotate_PositionX(float angle)
{
  QVector3D right = QVector3D::crossProduct(camUp,pView); // ищем вектор перпендикуляра
  rq = QQuaternion::fromAxisAndAngle(right,angle)*rq; // получаем дополнительный разворот матрицы поворота
}

// поворот от третьего лица Y
void CameraQuat::Rotate_PositionY(float angle)
{
  QVector3D top;
  if (grounded) { // если используем режим "заземления" то вращаем исключительно паралельно "земле"
      top = QVector3D(0,1,0);
    }
  else {
      top = camUp;
    }
  rq = QQuaternion::fromAxisAndAngle(top,angle)*rq; // получаем дополнительный разворот матрицы поворота
}

// повороты от первого лица
void CameraQuat::rotateByMouse(const QPointF &p)
{
    int msecs;                                  // дельта времени
    QTime currentTime = QTime::currentTime();   // взятие текущего времени
    msecs = m_lastTime.msecsTo(currentTime);

    if (msecs <= 20)    // лишний раз зачем считать?
        return;

    QLineF delta(m_lastPos, p);
    q_angularVelocity = 180*delta.length() / (PI*msecs);
    //*********************************************************
    //*/ этот кусок не учитывает поворот координат по Z, при повороте на 180 мышь получается инвертированной
    //*/ тоже самое если использовать функции turnOX, turnOY, turnOZ
    //*/ m_axis = QVector3D(-delta.dy(), delta.dx(), 0.0f).normalized();
    //*/ m_axis = transformation.rotatedVector(m_axis);
    //*/ q = QQuaternion::fromAxisAndAngle(m_axis, 180 / PI * delta.length()) * q;
    //*********************************************************
    /**/    turnLR(180 / PI * delta.dx()); // * mouse_sensitivity
    /**/    turnUD(180 / PI * delta.dy()); // * mouse_sensitivity
    //*********************************************************
    m_lastPos = p;              // запоминаем последнюю позицию мыши
    m_lastTime = currentTime;   // запоминаем последнюю позицию времени (для расчёта скорости вращения (пригодится))
}

void CameraQuat::moveByMouse(const QPointF &p)
{
    QLineF delta(m_lastPos, p);
    moveLR(delta.dx()); // * mouse_sensitivity
    moveUD(-delta.dy()); // * mouse_sensitivity
    m_lastPos = p;      // запоминаем последнюю позицию мыши
}

void CameraQuat::push(const QPointF& p)   // запоминаем сотояние (обычно при нажатии кнопки мыши)
{
    //m_rotation = rotation();            // запоминаем текущее значение поворота в кватернионе (с учётом прошедшего времени)
    //m_pressed = true;                   // ВЗВОДИМ флаг нажатия клавиш мыши
    m_lastTime = QTime::currentTime();  // запоминаем время когда запомнили текущий поворот (здесь возникает лаг с currentTime так как прошло время машшинной обработки команд и currentTime в функции rotation() != m_lastTime)
    m_lastPos = p;                      // запоминаем текущую точку в которой щёлкнули кнопкой мыши (пересчитанной из координат устройства в координаты окна отображения)
    //q_angularVelocity = 0.0f;           // устанавливаем угловую скорость равной 0 (тормозим вращение)
}
