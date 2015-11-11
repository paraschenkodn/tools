#include "mapbuilder.h"
//#include "buildermap.cpp"

MapBuilder::MapBuilder()
{
gridStep=0.1f;
currentmap=FLAT_MAP;
}

void MapBuilder::flatMap()
{
    // 1. открываем файл
    // 2. на его основе заполняем временную структуру данных для сцены
    // 3. устанавливаем начальный кадр во временной структуре
    // 4. переносим структуру из временной в рабочую для отображения на сцене (через критическую секцию)

    // TODO сделать выбор парсера для типа файла
    parserTRT();    // пока зададим жёсткий парсер

}

void MapBuilder::sphereMap()
{

}

void MapBuilder::newmap()
{
    filename=QFileDialog::getOpenFileName( 0, QString("Open File"));
    if (filename.isEmpty())
        return;
    //TODO проверка структуры файла

    //строим данные во временное хранилище данных
    //(если возникает ошибка сворачиваем всю работу с сообщением об ошибке)
    flatMap();
    sphereMap();
}

void MapBuilder::addPairHostsSt(QString &tr, QString &rec)
{ /// tr- трансивер (передатчик), rec - приёмник (receiver)
    // обрабатываем полученную структуру списка хостов и полученную пару типа string
        // проверяем, были ли такие IP
        int a=caption.indexOf(tr); //OldIp
        int b=caption.indexOf(rec); //Ip
        // проверяем совпадение пары на пару
        if ((b-a)==1||(a-b)==1){  // было, пропускаем, возможен и обратный вариант
            continue;
            }
        // проверяем, был ли адрес передатчика приёмником или передатчиком, или он новый
        // (а если множество приёмников и передатчиков) ПЕРЕБОРОМ? нет, точка одна, смотрим направление (приёмник или передатчик)
        if (a==-1&&b==-1) { // новый приёмник и передатчик
            // такое по идее может быть только с первой парой, т.к. у нас связанные последовательно данные, но мало ли, надо как-то отработать ситуацию
            caption.append(tr);
            caption.append(rec);
            // размещаем на координатной сетке
            if (true) { // TODO делаем проверку на незанятость начала координат (если это случилось то else ...)
              vertices.append(0.0f); vertices.append(0.0f); vertices.append(0.0f); // xyz tr
              vertices.append(0.0f+gridStep); vertices.append(0.0f+gridStep); vertices.append(0.0f+gridStep); // xyz rec
              }
        }
        if (a!=-1 && b==-1) // &-побитовое И // нечет - 1, а это значит был приёмником (чёт - передатчиком)
            { // старый адрес был передатчик или приёмник (не важно, точка уже была), и новый приёмник
        }
        if (a==-1 && b!=-1)
            { // новый передатчик, и точка приёмника уже была
        }
        if (a!=-1 && b!=-1)
            { // обе точки уже были, но они отстоят далеко друг от друга !!! трудный случай (оформлять как переход?)
            // отрисовывать возвратную линию, на расстоянии полшага по сетке?
        }
}

void MapBuilder::parserTRT()
{
    /// ПАРСЕР ФАЙЛА полученного tracert (windows) ("один-файл - один маршрут!") //TODO ещё допиливать и допиливать в поте лица
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly))
            return;
    QTextStream in(&file);
    QString line;
    //QString word;
    QRegExp rx("(([^\\[\\s]*)\\s+\\[)*((\\d{1,3}\\.){3}\\d{1,3})");
    QString Domain, Ip, OldIp("");
    while(!in.atEnd()) {
        line=in.readLine(); //
        // проверяем что строка относится к трассировке
        bool ok;
        line.mid(0,3).toInt(&ok,10);
        if (ok) { // всё в порядке, это строка трассировки
            // вычленяем из строки домен и IP
            int pos = rx.indexIn(line);
            if (pos>-1) { // это строка имеющая IP
                Domain = rx.cap(2); // если доменное имя не определилось, строка Domain будет пустой
                Ip = rx.cap(3);
                qDebug() << Domain << " " <<  Ip ;
            }
            else {  // строка без IP  * * *
                Domain=""; Ip="";
                qDebug() << "неизвестный хост";
                // TODO добавление фиктивных хостов
            }
            // добавляем данные для обработки в сырую структуру векторов (список IP устройств)
            hosts.append(OldIp);
            hosts.append(Ip);
            OldIp=Ip;
        }
      //in >> (word);
      }
    // удаляем первую пару, т.к. передатчик в ней нулевой
    if (!hosts.isEmpty()) // #1
        hosts.removeFirst();
    if (!hosts.isEmpty()) // #2
        hosts.removeFirst();
    file.close();
}

