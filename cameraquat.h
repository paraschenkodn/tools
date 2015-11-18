#ifndef CAMERAQUAT_H
#define CAMERAQUAT_H

#include <QOpenGLShaderProgram>
#include <QTime>
#include <qmath.h>

#define PI 3.14159265358979

class CameraQuat
{
public:
    CameraQuat();

    QVector3D pos;
    QQuaternion q;  // кватернион вращения
    QQuaternion rq; // кватернион вращения позиции камеры

    /*void apply(){
      QVector3D dir = q.rotatedVector(QVector3D(0,0,-1))+pos;
      QVector3D top = q.rotatedVector(QVector3D(0,1,0));
      gluLookAt(pos.x, pos.y, pos.z, dir.x, dir.y, dir.z, top.x, top.y, top.z);
    }//*/
    /*ALL VALUES IN GRAD*/
    void turnLR(float fi){                                  // поворот от первого лица лево-право
      QVector3D top = q.rotatedVector(QVector3D(0,1,0));
      q=QQuaternion::fromAxisAndAngle(top,fi)*q;
      /// q=QQuaternion::fromAxisAndAngle(top,fi)*q;  - аналогично двум последующим строкам:
      /// double fi2 = (double)fi*PI/360.0;
      /// q=QQuaternion((float)cos(fi2), top*(float)sin(fi2))*q;
    }

    void turnUD(float fi){                                      // поворот от первого лица вверх вниз
      QVector3D right =  q.rotatedVector(QVector3D(1,0,0));
      double fi2 = (double)fi*PI/360.0;
      q=QQuaternion((float)cos(fi2), right*(float)sin(fi2))*q;
    }

    void twirl(float fi){                                   // поворот от первого лица вокруг направления взгляда
      QVector3D dir =  q.rotatedVector(QVector3D(0,0,-1));
      double fi2 = (double)fi*PI/360.0;
      q=QQuaternion((float)cos(fi2), dir*(float)sin(fi2))*q;
    }

    void turnOX(float fi){  // поворот строго по осям системы координат (без учёта поворота самой системы координат)
      double fi2 = (double)fi*PI/360.0;
      q=QQuaternion((float)cos(fi2), QVector3D(1,0,0)*(float)sin(fi2))*q;
    }

    void turnOY(float fi){  // поворот строго по осям системы координат (без учёта поворота самой системы координат)
      double fi2 = (double)fi*PI/360.0;
      q=QQuaternion((float)cos(fi2), QVector3D(0,1,0)*(float)sin(fi2))*q;
    }

    void turnOZ(float fi){  // поворот строго по осям системы координат (без учёта поворота самой системы координат)
      double fi2 = (double)fi*PI/360.0;
      q=QQuaternion((float)cos(fi2), QVector3D(0,0,1)*(float)sin(fi2))*q;
    }

    void moveFB(float dist)     //  смещение камеры в сторону от первого лица, шаг вперёд-назад
    {
      QVector3D mdir =  q.rotatedVector(QVector3D(0,0,-1));
      mdir.normalize();
      pos += mdir*dist;
    }

    void moveLR(float dist)     //  смещение камеры в сторону от первого лица, шаг влево-вправо
    {
      QVector3D mdir =  q.rotatedVector(QVector3D(1,0,0));
      mdir.normalize();
      pos += mdir*dist;
    }

    void moveUD(float dist)     //  смещение камеры в сторону от первого лица, шаг вверх-вниз
    {
      QVector3D mdir =  q.rotatedVector(QVector3D(0,1,0));
      mdir.normalize();
      pos += mdir*dist;
    }

    void strafe(float fi){  // "следящая" камера
        ;
    }

    void Rotate_Position(float angle, float x, float y, float z);   // поворот камеры от третьего лица

    void push(const QPointF& p);
    void moveByMouse(const QPointF& p, const QQuaternion &transformation);
    QVector3D m_axis;           // ось вращения
    float m_angularVelocity;    // угловая скорость вращения
    QPointF m_lastPos;
    QTime m_lastTime;
    float mouse_sensitivity;
};

#endif // CAMERAQUAT_H
