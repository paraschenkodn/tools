#ifndef PLUGININTERFACE_H
#define PLUGININTERFACE_H

#include <QtCore>

/// в модули объявление попадает включением mainwindow.h либо прямым включением
class BuilderMapInterface // у плагина нет конструктора
{
public:
  virtual ~BuilderMapInterface(){}         // хочешь написать свой деструктор?

  virtual QStringList plushki() const = 0;  // список реализуемых плюшек (можно напихать в менюшек)

  virtual void newbmap(QString ID, int idNode) = 0; // это первая плюшка :)
  /// СТАВЯТСЯ В СООТВЕТСТВИЕ ИНДЕКСЫ ВЫБОРКИ И ИНДЕКСЫ ВЫБОРА ПЛАГИНА idVPlugin=IDv=iCaptions
  std::vector<float> *vertices;
  QVector<QString> *captions;
  std::vector<void *> *selectids; //idVPlugin

};

Q_DECLARE_INTERFACE(BuilderMapInterface, "com.3thf.Tools.BuilderMapInterface/1.0")

#endif // PLUGININTERFACE_H
