#include "karta.h"
#include "shphere.h"
#include "text.h"


Karta::Karta()
{

}

void Karta::draw()
{
  // РИСУЕМ ТРЕУГОЛЬНИК
  // инициализируем данные программы матрицы и атрибуты
  m_triangle->init();
  // зaпихиваем в его программу матрицу ориентации
  m_triangle->m_program.setUniformValue(m_triangle->m_matrixUniform, MVPM);
  // вызываем функцию рисования объекта (или объектов в for)
  m_triangle->draw();
  // проводим сброс инициализации параметров
  m_triangle->drop();//*/

  //РИСУЕМ СФЕРЫ
  m_shphere->init();
  m_shphere->m_program->setUniformValue(m_shphere->m_matrixUniform, MVPM);
  m_shphere->m_program->setUniformValue("PMi", PMi);                          // TODO вынести в класс шфер
  m_shphere->m_program->setUniformValue("MVM", MVM);
  m_shphere->m_program->setUniformValue("MVPMi", MVPMi);
  m_shphere->m_program->setUniformValue("viewport",viewport);
  m_shphere->draw();
  m_shphere->drop();//*/

  //РИСУЕМ ТЕКСТ
  //m_text->font=QFont::Bold;
  //m_text->pen=
  m_text->set();
  m_text->drawO(MVPM,"Точка 1",QVector3D(m_triangle->m_vertices[0],m_triangle->m_vertices[1],m_triangle->m_vertices[2]));
  m_text->drawO(MVPM,"Точка 2",QVector3D(m_triangle->m_vertices[3],m_triangle->m_vertices[4],m_triangle->m_vertices[5]));
  m_text->drawO(MVPM,"Точка 3",QVector3D(m_triangle->m_vertices[6],m_triangle->m_vertices[7],m_triangle->m_vertices[8]));
  m_text->reset();

}

