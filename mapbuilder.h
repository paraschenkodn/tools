#ifndef MAPBUILDER_H
#define MAPBUILDER_H

#include <QFileDialog>
#include <QTextStream>
#include <QDebug>
#include <QVector>

//#include "b.h"

#define FLAT_MAP    1
#define SPHERE_MAP  2

class MapBuilder
{
public:
    MapBuilder();

    void flatMap();     // строим плоскую карту
    void sphereMap();   // строим сферообразную карту
    void newmap();      // готовим новые данные для карт

    int currentmap;        // текущий вид карты
    QString filename;

    std::vector<float> vertices;    // пары координат (передатчик-приёмник)
    std::vector<QString>    data;   // собственно пара надписей на заборе для передачика и приёмника
    QVector<float>  coords;         //
    QVector<QString>    caption;    //

};

#endif // MAPBUILDER_H
