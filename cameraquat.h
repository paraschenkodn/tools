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

    QQuaternion q;  // кватернион вращения самой камеры (вид от первого лица)
    QQuaternion rq; // кватернион вращения вокруг взгляда камеры (вид от третьего лица)
    // позиция камеры (у нас она вначале отстоит по оси Z на +1, направление взгляда будет в сторону -Z)
    // у нас вектор позиции отсчитывается от центра координат
    QVector3D pos;  // перемещения камеры записываются в неё
    // вектор взгляда, направленный из точки позиции камеры в позицию вида от третьего лица, т.е.
    // это относительная позиция взгляда (у нас она вначале установлена в центр координат, в сторону -Z по оси)
    // у нас этот вектор отсчитывается от позиции камеры до точки взгляда
    QVector3D pView;  // изменяя размер этого вектора изменяем позицию камеры относительно объекта слежения
    // абсолютная позиция взгляда, является вычисляемой величиной от позиции и вектора взгляда (отсчёт от центра координат)
    QVector3D posView;
    // относительный единичный вектор, вычисляемая из кватерниона величина, вектор ориентации камеры вокруг своей оси
    QVector3D camUp;    // вектор ориентации бокового вращения камеры (не положения верха взгляда)
    // вектор направления движения камеры, размер вектора - скорость
    QVector3D speed;
    // матрица трансформации (самера)
    QMatrix4x4 CameraView; // наша ModelView matrix
    bool grounded;  // "заземление". вращение по горизонтали исключительно вокруг вертикальной оси Y
    bool strated; // режим полёта над заданной плоскостью, камера смотрит вниз, но движение вперёд не изменяет высоту по Y

    // функция установки точки слежения (например следящая за объектом камера)
    void setView(const QVector3D &npos){ // передаём абсолютные координаты вектора точки слежения (отсчёт вектора от центра координат)
      pView=npos-pos;
      posView=npos;
    }

    void apply(){ // функция актуализации вектора взгляда камеры после изменения кватернионов (не требуется после изменения самой точки pos)
      /// TODO сюда можно будет воткнуть критическую секцию
      // сначала определяем старую точку взгляда относительно смещеной позиции (если камера была сдвинута (а не повёрнута))
      posView=pos+pView;
      if (!q.isIdentity()) {  // вычисляем поворот точки взгляда
         posView = q.rotatedVector(pView)+pos; // новая позиция взгляда
         camUp = q.rotatedVector(camUp);  // новая позиция верха
         pView = posView-pos; // фиксируем новый вектор взгляда
        }
      if (!rq.isIdentity()) { // вычисляем поворот позиции камеры
         pos = rq.rotatedVector(-pView)+posView;
         pView = posView-pos; // фиксируем новый вектор взгляда
        }
      CameraView.setToIdentity();
      //CameraView.lookAt(pos,posView,camUp);
      QVector3D up;
      if (grounded) {
          up=QVector3D(0,1,0);  // камеру "держим" вертикально
      } else {
          up=camUp;
      }
      CameraView.lookAt(pos,posView,up);
      // позиции и вектор взгляда зафиксированы, проводим обнуление кватернионов
      q.setVector(0.0f,0.0f,0.0f);
      q.setScalar(1.0f);
      rq.setVector(0.0f,0.0f,0.0f);
      rq.setScalar(1.0f);
    }
    /*ALL VALUES IN GRAD*/
    void turnLR(float fi){                                  // поворот от первого лица лево-право
      QVector3D top;
      if (grounded) { // если используем режим "заземления" то вращаем исключительно паралельно "земле"
          //top = q.rotatedVector(QVector3D(0,1,0));  // относительный вектор вращения
          top = QVector3D(0,1,0);
        }
      else {
        ///camUp = q.rotatedVector(camUp); // корректируем вектор вращения если уже был поворот ??? если фиксировать то всё сразу на каждом этапе, перед, или после.
        top = camUp;
        }
      q=QQuaternion::fromAxisAndAngle(top,fi)*q;
      /// q=QQuaternion::fromAxisAndAngle(top,fi)*q;  - аналогично двум последующим строкам:
      /// double fi2 = (double)fi*PI/360.0;
      /// q=QQuaternion((float)cos(fi2), top*(float)sin(fi2))*q;
    }

    void turnUD(float fi){                                      // поворот от первого лица вверх вниз
      //QVector3D right =  q.rotatedVector(QVector3D(1,0,0));
      /// camUp = q.rotatedVector(camUp);   // актуализируем поворот и всё остальное на каждом этапе
      /// TODO pView если уже был незафиксированный поворот, актуализируем его
      // ищем вектор перпендикуляра
      QVector3D right = QVector3D::crossProduct(camUp,pView);
      // смещаем относительно вектора перпендикуляра
      q=QQuaternion::fromAxisAndAngle(right,fi)*q;
    }

    void twirl(float fi){                                   // поворот от первого лица вокруг направления взгляда
      //QVector3D dir =  q.rotatedVector(QVector3D(0,0,-1));
      /// TODO pView // корректируем если был поворот
      if (grounded) {
          ;
      } else {
          q=QQuaternion::fromAxisAndAngle(pView,fi)*q;
      }
    }

    void turnOX(float fi){  // поворот строго по осям системы координат (без учёта поворота самой системы координат)
      q=QQuaternion::fromAxisAndAngle(QVector3D(1,0,0),fi)*q; // поворот мира вокруг оси Х центра координат
    }

    void turnOY(float fi){  // поворот строго по осям системы координат (без учёта поворота самой системы координат)
      q=QQuaternion::fromAxisAndAngle(QVector3D(0,1,0),fi)*q;
    }

    void turnOZ(float fi){  // поворот строго по осям системы координат (без учёта поворота самой системы координат)
      q=QQuaternion::fromAxisAndAngle(QVector3D(0,0,1),fi)*q;
    }

    void moveFB(float dist)     //  смещение камеры в сторону от первого лица, шаг вперёд-назад
    {
      QVector3D mdir =  pView;
      mdir.normalize();
      pos += mdir*dist;
    }

    // strafe
    void moveLR(float dist)     //  смещение камеры в сторону от первого лица, шаг влево-вправо
    {
      // если уже был незафиксированный поворот, актуализируем его
      /// camUp = q.rotatedVector(camUp); актуализируем всё сразу, перед, или после
      /// TODO pView если уже был незафиксированный поворот, актуализируем его
      // ищем вектор перпендикуляра и смещаем относительно его
      QVector3D mdir = QVector3D::crossProduct(camUp,pView);
      mdir.normalize();
      pos += mdir*dist;
    }

    void moveUD(float dist)     //  смещение камеры в сторону от первого лица, шаг вверх-вниз
    {
      //QVector3D mdir =  q.rotatedVector(QVector3D(0,1,0));
      //mdir.normalize();
      // если уже был незафиксированный поворот, актуализируем его
      /// TODO актуализация camUp = q.rotatedVector(camUp);   // camUp у нас изначально нормализованный вектор
      QVector3D mdir = camUp;
      if (strated){   // режим парящей камеры
          mdir.setY(0);
        }
      pos += mdir*dist; //pos += mdir*dist;
    }

    // поворот камеры от третьего лица
    void Rotate_PositionX(float angle);
    void Rotate_PositionY(float angle);

    void push(const QPointF& p);
    void rotateByMouse(const QPointF& p);
    void moveByMouse(const QPointF& p);

    QVector3D q_axis;           // ось вращения
    float q_angularVelocity;    // угловая скорость вращения позиции камеры
    QVector3D rq_axis;           // ось вращения
    float rq_angularVelocity;    // угловая скорость вращения вокруг объекта слежения

    QPointF m_lastPos;
    QTime m_lastTime;
    float mouse_sensitivity;

    void reset();
};

#endif // CAMERAQUAT_H
