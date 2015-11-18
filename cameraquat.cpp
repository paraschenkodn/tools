#include "cameraquat.h"
#include <QLineF>

CameraQuat::CameraQuat()
{
q.setVector(0.0f,0.0f,0.0f);
q.setScalar(1.0f);
pos.setX(0.0f);pos.setY(0.0f);pos.setZ(0.0f);
rq.setVector(0.0f,0.0f,0.0f);
rq.setScalar(1.0f);

m_angularVelocity=0;
m_axis = QVector3D(0, 1, 0);            // начальный вектор вращения вдоль оси Y
m_lastTime = QTime::currentTime();      // зафиксируем текущее время (для вычислений с угловой скоростью)
mouse_sensitivity=1.0f;
}

void CameraQuat::Rotate_Position(float angle, float x, float y, float z)
{
    //
  // получаем вектор поворота
  QVector3D axis = QVector3D(x,y,z); //
  // получаем обратный вектор, от позиции места куда падает взгляд до позиции камеры.
  QVector3D mPos = -(q.rotatedVector(QVector3D(0,0,-1)));
  // делаем квартерион поворота вокруг вектора поворота
  QQuaternion nq=QQuaternion::fromAxisAndAngle(axis,angle);//*q;
  // получаем суммирующий квартерион поворота позиции
  rq=rq*nq;
  // получаем повёрнутый вектор позиции камеры
  //QVector3D npos=rq.rotatedVector(mPos);
  pos=rq.rotatedVector(mPos);
  // (но вектор взгляда теперь смотрит с новой позиции в сторону)
  // теперь поворачиваем вектор взгляда (т.е. камеру) к точке просмотра
  /// TODO (пока )
  turnLR(angle);

    //"позиция камеры" - "позиция взгляда(ось вращения будет там куда смотрим)"
  // получаем вектор
    QVector3D mView = q.rotatedVector(QVector3D(0,0,-1))+pos;

    QVector3D top = q.rotatedVector(QVector3D(0,1,0));  // это вращение по y получится, потому используем ориентацию axis

    QVector3D dir = q.rotatedVector(axis)+pos;  // используем ориентацию axis
    //*/

    /*/
    //"позиция камеры" - "позиция взгляда(ось вращения будет там куда смотрим)"
    QVector3D mView = q.rotatedVector(QVector3D(0,0,-1))+pos;
    pos.setX( pos.x() - mView.x());
    pos.setY( pos.y() - mView.y());
    pos.setZ( pos.z() - mView.z());

    QVector3D vVector = pos;
    QVector3D AVector;

    float SinA = (float)sin(PI * angle / 180.0);
    float CosA = (float)cos(PI * angle / 180.0);

    // Найдем новую позицию X для вращаемой точки
    AVector.setX((CosA + (1 - CosA) * x * x) * vVector.x());
    AVector.setX(AVector.x() + ((1 - CosA) * x * y - z * SinA) * vVector.y());
    AVector.setX(AVector.x() + ((1 - CosA) * x * z + y * SinA) * vVector.z());

     // Найдем позицию Y
    AVector.setY(((1 - CosA) * x * y + z * SinA) * vVector.x());
    AVector.setY(AVector.y() + (CosA + (1 - CosA) * y * y) * vVector.y());
    AVector.setY(AVector.y() + ((1 - CosA) * y * z - x * SinA) * vVector.z());

     // И позицию Z
    AVector.setZ(((1 - CosA) * x * z - y * SinA) * vVector.x());
    AVector.setZ(AVector.z() + ((1 - CosA) * y * z + x * SinA) * vVector.y());
    AVector.setZ(AVector.z() + (CosA + (1 - CosA) * z * z) * vVector.z());

    pos.setX( mView.x() + AVector.x());
    pos.setY( mView.y() + AVector.y());
    pos.setZ( mView.z() + AVector.z());//*/
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
