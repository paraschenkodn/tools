attribute highp vec4 vertexAttr; // входной аттрибут для координат вершин
uniform highp mat4 MVPM;        // матрица положения средней точности задающая позицию вершин
attribute highp vec2 texAttr;  // // входной аттрибут для координат цветов
varying highp vec2 texCoord;  // общая переменная для вершин (выходной параметр)

void main () {
    gl_Position = MVPM*vertexAttr; // устанавливаем позицию вершин через матрицу
    texCoord = texAttr; // и передаём цвет в фрагментный шейдер
}
