#ifndef MAPBUILDER_H
#define MAPBUILDER_H

#include <QFileDialog>
#include <QTextStream>
#include <QDebug>
#include <QVector>
#include <QMutex>

#define FLAT_MAP    1
#define SPHERE_MAP  2

class MapBuilder
{
public:
    MapBuilder();
    QMutex m_mutex;

    bool flatMap(bool clear);     // строим плоскую карту
    bool sphereMap(bool clear);   // строим сферообразную карту
    void newmap(bool clear);      // готовим новые данные для карт

    void parserTRT();   // парсер данных команды tracert

    int currentmap;        // текущий вид карты
    bool newmapbuild;      // новая карта готова для отображения (после копирования данных скидывается)
    QString filename;

    QVector<QString>    hosts;      // временный СЫРОЙ СПИСОК (из парсера) собственно пара надписей на заборе для передачика и приёмника
    QVector<QString>    pairs;    // общий список пар хостов в сети
    // выходные данные
    QVector<float>    vertices;         // пары точек (передатчик-приёмник)
    QVector<QString>    captions; //надписи для пар точек

    // ********* ПЛОСКАЯ КАРТА ********** функции и переменные
    void fmapPairHostsSt(QString &tr, QString &rec);    // картирование пары хостов (добавление на карту пары передатчик-приёмник)
    void fmapToVertices();                          // сброс координатной сетки в координаты вершин
    void flatMapPairHostsSt();                          // TODO картирование напрямую в координатной сетке, без использования массива fmap
    QVector< QVector<QString> > fmap;   // координатная сетка точек (индексов точек пар) служит для построения
    float gridStep;     // расстояние между узлами
    int sizefmap;   // размер условной координатной сетки
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
