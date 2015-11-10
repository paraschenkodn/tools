#include "mapbuilder.h"
//#include "b.cpp"

MapBuilder::MapBuilder()
{
currentmap=FLAT_MAP;
}

void MapBuilder::flatMap()
{
    // 1. открываем файл
    // 2. на его основе заполняем временную структуру данных для сцены
    // 3. устанавливаем начальный кадр во временной структуре
    // 4. переносим структуру из временной в рабочую для отображения на сцене (через критическую секцию)
    /// ПАРСЕР ФАЙЛА полученного tracert (windows)  //TODO ещё допиливать и допиливать в поте лица
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
            }
            // TODO проверяем, были ли такие IP
            // добавляем данные для обработки в сырую структуру векторов (список IP устройств)
            //caption.append(OldIp);
            caption.append(Ip);
            OldIp=Ip;
        }
      //in >> (word);
      }
    // обрабатываем полученную структуру списка хостов
    for (int i=0;i<caption.size();i++){
        // устанавливаем начальный хост
    }
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

