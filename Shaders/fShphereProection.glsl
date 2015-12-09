//#version 300 es
//precision highp float;

uniform highp vec4 viewport;

//uniform highp mat4 VPInverse;
varying highp mat4 VPInverse;

varying highp mat4 VPMTInverse;
varying vec3 centernormclip;

varying highp vec4 IDf;  // идентификатор обрабатываемой вершины (фрагмента?)
uniform bool selectmode;  //    режим выбора true - выбор в шейдере, false - нет (selectID может не передаваться)
uniform vec2 mouse; //  координаты выборки

//layout ( pixel_center_integer ) in vec4 gl_FragCoord;  // модификатор необходим чтобы сравнивать с координатами мыши 1:1

void main(void) {
        highp vec4 c3 = VPMTInverse[2];
        highp vec4 xpPrime = VPMTInverse * highp vec4(gl_FragCoord.x, gl_FragCoord.y, 0.0, 1.0);

        float c3TDc3 = dot(c3.xyz, c3.xyz)-c3.w*c3.w;
        float xpPrimeTDc3 = dot(xpPrime.xyz, c3.xyz)-xpPrime.w*c3.w;
        float xpPrimeTDxpPrime = dot(xpPrime.xyz, xpPrime.xyz)-xpPrime.w*xpPrime.w;

        float square = xpPrimeTDc3*xpPrimeTDc3 - c3TDc3*xpPrimeTDxpPrime;
        if (square<0.0) {
            discard;
        } else {
            float z = ((-xpPrimeTDc3-sqrt(square))/c3TDc3);
            gl_FragDepth = z;

            highp vec4 pointclip = VPInverse * highp vec4(gl_FragCoord.x, gl_FragCoord.y, z, 1);
            vec3 pointnormclip = vec3(pointclip)/pointclip.w;

            vec3 lightDir = normalize(vec3(gl_LightSource[0].position));
            float intensity = .2 + max(dot(lightDir,normalize(pointnormclip-centernormclip)), 0.0);
            gl_FragColor = intensity*gl_Color;
        }
    // кусок кода для выборки
    if (selectmode) {   // заполняем цветом
        gl_FragColor=IDf;
    }
    // *****
}
