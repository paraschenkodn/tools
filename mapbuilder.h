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

    void addPairHostsSt(QString &tr, QString &rec);    // добавление пары хостов (передатчик-приёмник)
    void parserTRT();   // парсер данных команды tracert

    int currentmap;        // текущий вид карты
    QString filename;

    QVector<QString>    hosts;      // СЫРОЙ СПИСОК (из парсера) собственно пара надписей на заборе для передачика и приёмника
    QVector<float>    vertices;         // пары координат (передатчик-приёмник)
    QVector<QString>    caption;    // надписи для точек

    // ********* ПЛОСКАЯ КАРТА ********** функции и переменные
    QVector< QVector<QString> > fmap;   // координатная сетка точек (индексов точек пар) служит для построения
    float gridStep;     // шаг сетки координат
    int sizefmap;   // размер координатной сетки
    void resizemap()
    {
        fmap.resize(sizefmap);
        for (int i=0;i<sizefmap;i++) {
            fmap[i].resize(sizefmap);
         }
    }   // только в сторону увеличения
    bool getpointfmap(QString &data, int &row, int &col);     // найти заданную точку по данным  (дата row col), возвращает row col, bool - true if found
};

//#include "buildermap.h"

#endif // MAPBUILDER_H
