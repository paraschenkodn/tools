#ifndef MAPBUILDER_H
#define MAPBUILDER_H

#include <QFileDialog>

#define FLAT_MAP    1
#define SPHERE_MAP  2

class MapBuilder
{
    Q_OBJECT
public:
    MapBuilder();

    void flatMap();     // строим плоскую карту
    void sphereMap();   // строим сферообразную карту
    void newmap();      // готовим новые данные для карт

    int currentmap;        // текущий вид карты
    QString filename;
};

#endif // MAPBUILDER_H
