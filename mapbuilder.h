#ifndef MAPBUILDER_H
#define MAPBUILDER_H

#include <QFileDialog>
#include <QTextStream>
#include <QDebug>
#include <QVector>

#define FLAT_MAP    1
#define SPHERE_MAP  2

class MapBuilder
{
public:
    MapBuilder();

    void flatMap();     // строим плоскую карту
    void sphereMap();   // строим сферообразную карту
    void newmap();      // готовим новые данные для карт

    void addPairHostsSt(QString &, QString &);    // добавление пары хостов (передатчик-приёмник)
    float gridStep;     // шаг сетки координат
    void parserTRT();   // парсер данных команды tracert

    int currentmap;        // текущий вид карты
    QString filename;

    QVector<QString>    hosts;      // СЫРОЙ СПИСОК (из парсера) собственно пара надписей на заборе для передачика и приёмника
    QVector<float>      vertices;         // пары координат (передатчик-приёмник)
    QVector<QString>    caption;    // надписи для точек

};

//#include "buildermap.h"

#endif // MAPBUILDER_H
