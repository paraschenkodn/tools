#include "level.h"

Level::Level()
{
  // устанавливаем параметры сцены
  near_=0.001f;
  far_=100.0f;
  range=far_-near_;

  // устанавливаем параметры камеры
  reset();  // init level
  setCamera();
}

void Level::reset(){    // init level
    //  параметры камеры
    camera.reset();// сброс камеры
    cameraFocusAngle=40;                // устанавливаем начальный угол проекции
    camera.pos=QVector3D(0.0f,0.0f,0.26f);
    camera.setView(QVector3D(0.0f,0.0f,0.0f));
    setCamera(); // устанавливаем параметры камеры
}

void Level::setCamera() {
    camera.apply();
    MVM=camera.CameraView;
    setCamInfo();
}

void Level::setCamInfo()
{
  /// TODO вычисляемые параметры углов поворота камеры
  ///int angle_x; // текущий угол поворота камеры по X (матрицы поворота CameraView, она же ModelView)
  ///int angle_y; // текущий угол поворота по Y
  ///int angle_z; // текущий угол поворота по Z
    QString text="I,O - изменение проекции. Угол проекции="+QString().setNum(cameraFocusAngle);
    text=text+" cameraEye="+QString().setNum(camera.pos[0])+", "+QString().setNum(camera.pos[1])+", "+QString().setNum(camera.pos[2])
            +", cameraCenter="+QString().setNum(camera.posView[0])+", "+QString().setNum(camera.posView[1])+", "+QString().setNum(camera.posView[2])
            +" cameraUp="+QString().setNum(camera.camUp[0])+", "+QString().setNum(camera.camUp[1])+", "+QString().setNum(camera.camUp[2]);
    emit setCameraInfo(text);
}
