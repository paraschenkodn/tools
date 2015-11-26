//#version 300 es
//precision highp float;

uniform vec4 viewport;

varying mat4 VPMTInverse;
varying mat4 VPInverse;
varying vec3 centernormclip;
varying vec4 IDf;  // идентификатор обрабатываемой вершины (фрагмента?)
vec4 tmpIDf;  // идентификатор обрабатываемой вершины (фрагмента?)
vec4 IDfCoord;  // координаты запоминаемого фрагмента
bool selected;

uniform bool selectmode;  //    режим выбора true - выбор в шейдере, false - нет (selectID может не передаваться)
uniform vec2 mouse; //  координаты выборки

layout ( pixel_center_integer ) in vec4 gl_FragCoord;  // модификатор необходим чтобы сравнивать с координатами мыши 1:1

void main(void) {
    // кусок кода для выборки
    selected=false;
    if (selectmode && gl_FragCoord.x==mouse.x && gl_FragCoord.y==mouse.y) {
        tmpIDf=IDf; // запоминаем объект который совпал
        IDfCoord=gl_FragCoord; // его координаты
        selected=true;
    }
    // *****

    vec4 c3 = VPMTInverse[2];
    vec4 xpPrime = VPMTInverse*vec4(gl_FragCoord.x, gl_FragCoord.y, 0.0, 1.0);

    float c3TDc3 = dot(c3.xyz, c3.xyz)-c3.w*c3.w;
    float xpPrimeTDc3 = dot(xpPrime.xyz, c3.xyz)-xpPrime.w*c3.w;
    float xpPrimeTDxpPrime = dot(xpPrime.xyz, xpPrime.xyz)-xpPrime.w*xpPrime.w;

    float square = xpPrimeTDc3*xpPrimeTDc3 - c3TDc3*xpPrimeTDxpPrime;
    if (square<0.0) {
        discard;
    } else {
        float z = ((-xpPrimeTDc3-sqrt(square))/c3TDc3);
        gl_FragDepth = z;

        vec4 pointclip = VPInverse*vec4(gl_FragCoord.x, gl_FragCoord.y, z, 1);
        vec3 pointnormclip = vec3(pointclip)/pointclip.w;

        vec3 lightDir = normalize(vec3(gl_LightSource[0].position));
        float intensity = .2 + max(dot(lightDir,normalize(pointnormclip-centernormclip)), 0.0);
        gl_FragColor = intensity*gl_Color;
    }

    // кусок кода для выборки
    if (selected) {
        gl_FragColor.x=IDf.x; // запоминаем ИД модели объекта
        gl_FragColor.y=IDf.y; // запоминаем ИД fragmenta
        gl_FragColor.z=gl_FragCoord.z;  // запоминаем глубину выбранного фрагмента, для последующей селекции (?? как прочитать в шейдере)
    } else {
        // даём понять что ничего не выбрано
        gl_FragColor.w=3711;
    }
    gl_FragCoord.z=0; // запоминаем выборку в 0 координате глубины, чтобы нарисовалась и мы потом прочитали (используется альфаканал, должен включен бленд)
    // *****
}
