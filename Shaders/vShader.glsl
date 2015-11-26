attribute highp vec4 vertexAttr; // входной аттрибут для координат вершин
uniform mediump mat4 MVPM;        // матрица положения средней точности задающая позицию вершин
attribute lowp vec4 colorAttr;  // // входной аттрибут для координат цветов
uniform lowp vec4 statcolor;  // статичный цвет рисования (aka glColor)
uniform bool colormode;  // режим рисования цвета
varying lowp vec4 color;  // общая переменная для вершин (выходной параметр)
attribute highp vec2 texAttr;  // // входной аттрибут для координат цветов
varying highp vec2 texCoord;  // общая переменная для вершин (выходной параметр)

void main () {
    gl_Position = MVPM * vertexAttr; // устанавливаем позицию вершин через матрицу
    if (colormode) {
        color=statcolor;
    } else {
        color = colorAttr; // и передаём цвет в фрагментный шейдер
    }
    //texCoord = texAttr; // и передаём цвет в фрагментный шейдер
}
