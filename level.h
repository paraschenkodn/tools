#ifndef LEVEL_H
#define LEVEL_H
/// Каждый уровень (отдельная сцена) содержит:
/// 1. состояние камеры и матриц преобразования для текущего уровня
/// 2. алгоритм отображения моделей и их поведение и взаимодействие на сцене - level_draw(DRAW_LAW)
/// 3. массив отображаемых моделей, которые в свою очередь содержат:
/// a. массив моделей-примитивов для отображения модели
/// б. функции и алгоритм рисования модели, её отображения model_draw(DRAW_LAW)
/// в. состояния и алгоритмы трансформации модели
/// с. примитивы должны содержать информацию:
/// I. режим отрисовки примитива (DRAW_NORM | DRAW_SFBO ... etc)
/// II. настраиваемые параметры *****
/// DRAW_LAW - задаёт режим рисования, например DRAW_SFBO задаёт режим рисования в буфер выбора объектов

/// В нашей карте пока один уровень отображения сцены, который реализован целиком в классе Scene

#define DRAW_NORM   1   // рисование в буфер рендеринга
#define DRAW_SFBO   2   // рисование в буфер "выбора"
#define DRAW_SOME   4   // резерв

#include "cameraquat.h"

class Level : public QObject
{
  Q_OBJECT
public:
    Level();

    QVector4D viewport;
    float near_, far_, range;

    // матрицы преобразований
    QMatrix4x4 LMM; // Local Model matrix (делает преобразования в локальных координатах объекта, для одного объекта их может быть несколько для разных частей объекта)
    QMatrix4x4 MM; // Model matrix (выносит координаты объекта в координаты пространства сцены,
                  //выполняется в следующем порядке - масштабирование, поворот, перенос)
                  // TranslationMatrix * RotationMatrix * ScaleMatrix * OriginalVector; (в коде это выглядит в обратном порядке)
    QMatrix4x4 MVM; //WorldView; // ModelView matrix (View matrix)("масштабирует крутит и перемещает весь мир")
    QMatrix4x4 CameraView; // тоже самое что и MVM, но для использования функции LookAt
    QMatrix4x4 PM; // Projection matrix // проекционная матрица
    QMatrix4x4 MVPM; // ModelViewProjection matrix (projection * view * model)
    // инверсные матрицы
    QMatrix4x4 PMi;
    QMatrix4x4 MVPMi;

    // переменные камеры
    CameraQuat camera;
    float ratio;
    QVector3D cameraEye;
    QVector3D cameraCenter;
    QVector3D cameraUp;
    QQuaternion cameraOrientation; // итоговый вектор ориентации камеры
    float cameraFocusAngle;
    //int camEyePos; // позиция камеры () // + control of board??
    void setCamera();  // установка параметров матрицы CameraView
    void setCameraInfo();  // формируем и посылаем текст для отображения параметров в главном окне
    // енд камера

    void reset();
    void setCamInfo();

signals:
    void setCameraInfo(const QString &);
};

#endif // LEVEL_H
