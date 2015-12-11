#version 120
uniform vec4 viewport;
uniform highp mat4 PM;  // gl_ProjectionMatrix
//varying lowp vec4 color;
varying float radius;
varying vec3  center;
varying vec4  position; // with w
varying vec3 centernormclip;

void main(void) {
    vec4 ndcPos;
    ndcPos.xy = ((gl_FragCoord.xy / viewport.zw)*2.0) - 1.0;
    ndcPos.z = (2.0 * gl_FragCoord.z - gl_DepthRange.near - gl_DepthRange.far) / (gl_DepthRange.far - gl_DepthRange.near);
    ndcPos.w = 1.0;
    vec4 current_pixel = ndcPos / gl_FragCoord.w  ;  //приводим к экранным координатам

    vec2 diff = current_pixel.xy - center.xy;  // вычисляем разницу векторов между точкой и центром

    vec3 current_center = center;

    current_pixel = gl_FragCoord;
            ndcPos.x = position.x / position.w;
            ndcPos.y = position.y / position.w;
            ndcPos.z = position.z / position.w;
            current_center.x = ndcPos.x * viewport.z/2 + (viewport.x + viewport.z/2);
            current_center.y = ndcPos.y * viewport.w/2 + (viewport.y + viewport.w/2);
            current_center.z = gl_DepthRange.diff/2.0 * ndcPos.z + (gl_DepthRange.near + gl_DepthRange.far)/2;
    diff = gl_FragCoord.xy - current_center.xy; //*/

    float d2 = dot(diff,diff);              // скалярное произведение вектора разницы на себя даёт размер умноженного вектора
    float r2 = radius*radius;               // радиус на радиус, теперь можем сравнить два скалярных произведения

    //gl_FragColor = vec4(1.0, 0.0, 1.0, 1.0);
    if (d2>r2) {        // если скаляр радиуса меньше скаляра вектора от центра сферы до точки поверхности, отбрасываем, иначе рисуем сферу
        discard;
    } else {
        vec3 l = normalize(gl_LightSource[0].position.xyz); // нормализованный вектор направления на позицию источника света
        float dr =  sqrt(r2-d2);    // размер вектора по оси Z
        vec3 n = vec3((current_pixel.xy - current_center.xy), dr);  // направление вектора нормали проходящей через текущий пиксель
        float intensity = .2 + max(dot(l,normalize(n)), 0.0);   // как только достигается перпендикулярность и далее dot<0.0, "так наступает тёмная сторона силы" 0.2
        gl_FragColor = gl_Color*intensity;
        gl_FragDepth =  gl_FragCoord.z - dr;// - dr*gl_DepthRange.diff/2.0*gl_ProjectionMatrix[2].z;
        //gl_FragColor = vec4(1.0, 0.0, 1.0, 1.0);
    }
    //gl_FragColor = gl_Color;
}
