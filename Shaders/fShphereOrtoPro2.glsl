//#version 120
//uniform highp mat4 PM;  // gl_ProjectionMatrix
uniform vec4 viewport;
varying float radius;
varying vec4  position; // with w
//uniform float near;
//uniform float far;
uniform float range;

varying highp vec4 IDf;  // идентификатор обрабатываемой вершины (фрагмента?)
uniform bool selectmode;  //    режим выбора true - выбор в шейдере, false - нет (selectID может не передаваться)
uniform vec2 mouse; //  координаты выборки

void main(void) {
    vec4 ndcPos;    // координаты центра сферы в координатах NDC
    float dr;    // размер вектора по оси Z
    vec4 current_pixel;  // в экранных координатах
    vec2 diff;  // разница векторов между точкой и центром
    vec3 current_center;
    //float gl_FragCoord_z=gl_FragCoord.z;
    float z;

    current_pixel = gl_FragCoord;
    ndcPos.x = position.x / position.w; // переводим координаты вертекса (центра окружности) из clip соординат в NDC координаты
    ndcPos.y = position.y / position.w;
    ndcPos.z = position.z / position.w;
    current_center.x = ndcPos.x * viewport.z/2.0 + (viewport.x + viewport.z/2.0);   // переводим координаты вертекса из NDC координат в экранные координаты
    current_center.y = ndcPos.y * viewport.w/2.0 + (viewport.y + viewport.w/2.0);
    // current_center.z = gl_FragCoord.z // current_center.z = (gl_DepthRange.diff/2.0) * ndcPos.z + (gl_DepthRange.near + gl_DepthRange.far)/2.0;  // координаты scene depth range
    diff = gl_FragCoord.xy - current_center.xy; //*/

    float d2 = dot(diff,diff);              // скалярное произведение вектора разницы на самого себя даёт размер вектора в квадрате
    float r2 = radius*radius;               // радиус в квадрате, теперь можем сравнить два произведения

    if (d2>r2) {        // если квадрат радиуса меньше квадрата вектора от центра сферы до точки поверхности, отбрасываем отображение пикселя, иначе рисуем сферу
        discard;
    } else {
        vec3 l = normalize(gl_LightSource[0].position.xyz); // нормализованный вектор направления на позицию источника света
        dr =  sqrt(r2-d2);    // размер вектора, коллинеарный оси Z экранных координат
        vec3 n = vec3((current_pixel.xy - current_center.xy), dr);  // направление вектора нормали проходящей через текущий пиксель от центра сферы
        float intensity = .2 + max(dot(l,normalize(n)), 0.0);   // как только достигается перпендикулярность и далее dot<=0.0, "так наступает тёмная сторона силы" =0.2
        gl_FragColor = gl_Color*intensity;

        z=(gl_DepthRange.diff/2.0) * (ndcPos.z-(dr/range)) + (gl_DepthRange.near + gl_DepthRange.far)/2.0;
        gl_FragDepth = z;

    }
    // кусок кода для выборки
    if (selectmode) {   // заполняем цветом выборки объекта
        gl_FragColor=IDf;
        //debug
        gl_FragColor.x = dr;
        gl_FragColor.y = z;
        gl_FragColor.z = gl_FragCoord.z;
        gl_FragColor.w = ndcPos.z;
        //*///debug
    }
}
