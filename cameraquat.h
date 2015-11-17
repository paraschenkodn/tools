#ifndef CAMERAQUAT_H
#define CAMERAQUAT_H

#include <QOpenGLShaderProgram>
#include <qmath.h>

#define PI 3.14159265358979

class CameraQuat
{
public:
    CameraQuat();

    QVector3D pos;
    QQuaternion q;

    /*void apply(){
      QVector3D dir = q.rotatedVector(QVector3D(0,0,-1))+pos;
      QVector3D top = q.rotatedVector(QVector3D(0,1,0));
      gluLookAt(pos.x, pos.y, pos.z, dir.x, dir.y, dir.z, top.x, top.y, top.z);
    }//*/
    /*ALL VALUES IN GRAD*/
    void turnLR(float fi){
      QVector3D top = q.rotatedVector(QVector3D(0,1,0));
      double fi2 = (double)fi*PI/360.0;
      q=QQuaternion((float)cos(fi2), top*(float)sin(fi2))*q;
    }

    void turnUD(float fi){
      QVector3D right =  q.rotatedVector(QVector3D(1,0,0));
      double fi2 = (double)fi*PI/360.0;
      q=QQuaternion((float)cos(fi2), right*(float)sin(fi2))*q;
    }

    void twirl(float fi){
      QVector3D dir =  q.rotatedVector(QVector3D(0,0,-1));
      double fi2 = (double)fi*PI/360.0;
      q=QQuaternion((float)cos(fi2), dir*(float)sin(fi2))*q;
    }

    void turnOX(float fi){
      double fi2 = (double)fi*PI/360.0;
      q=QQuaternion((float)cos(fi2), QVector3D(1,0,0)*(float)sin(fi2))*q;
    }

    void turnOY(float fi){
      double fi2 = (double)fi*PI/360.0;
      q=QQuaternion((float)cos(fi2), QVector3D(0,1,0)*(float)sin(fi2))*q;
    }

    void turnOZ(float fi){
      double fi2 = (double)fi*PI/360.0;
      q=QQuaternion((float)cos(fi2), QVector3D(0,0,1)*(float)sin(fi2))*q;
    }

    void moveFB(float dist)
    {
      QVector3D mdir =  q.rotatedVector(QVector3D(0,0,-1));
      mdir.normalize();
      pos += mdir*dist;
    }

    void moveLR(float dist)
    {
      QVector3D mdir =  q.rotatedVector(QVector3D(1,0,0));
      mdir.normalize();
      pos += mdir*dist;
    }

    void moveUD(float dist)
    {
      QVector3D mdir =  q.rotatedVector(QVector3D(0,1,0));
      mdir.normalize();
      pos += mdir*dist;
    }

};

#endif // CAMERAQUAT_H
