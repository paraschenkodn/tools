#ifndef PLUGININTERFACE_H
#define PLUGININTERFACE_H

#include <QtCore>

class BuilderMapInterface // у плагина нет конструктора
{
public:
  virtual ~BuilderMapInterface(){}         // хочешь написать свой деструктор?

  virtual QStringList plushki() const = 0;  // список реализуемых плюшек (можно напихать в менюшек)

  //virtual void newbmap(QString ID, double idNode, std::vector<float> *vertices, std::vector<float> *captions); // это первая плюшка :)
  virtual void newbmap(QString ID, double idNode) = 0; // это первая плюшка :)
  std::vector<float> *vertices;
  QVector<QString> *captions;

};

Q_DECLARE_INTERFACE(BuilderMapInterface, "com.3thf.Tools.BuilderMapInterface/1.0")

#endif // PLUGININTERFACE_H
