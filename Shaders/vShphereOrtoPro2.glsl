//#version 120
attribute highp vec4 vertexAttr;    //
attribute lowp vec4 colorAttr;
uniform highp mat4 MVPM;  // gl_ModelViewProjectionMatrix
uniform float R;    // радиус сферы
//uniform float maxpointsize; // системная величина, зависит от карты (возможно только для старых ATI)
uniform vec4 viewport; // //получаем размеры окна рисования (x0,y0,w,h)

attribute highp vec4 selectID;  // идентификаторы выборки
varying highp vec4 IDf;  // идентификатор обрабатываемой вершины (фрагмента)

varying float radius;   // расчитанный радиус сферы в точке перспективонй проекции
varying vec4  position; // координаты центра сферы (вершины)

void main()
{
gl_Position = MVPM * vertexAttr; // первоначальное положение , затем будем трансформировать с учётом перспективного искажения шара
gl_FrontColor = colorAttr;
IDf=selectID;

    mat4 T = mat4(
            1.0, 0.0, 0.0, 0.0,
            0.0, 1.0, 0.0, 0.0,
            0.0, 0.0, 1.0, 0.0,
            vertexAttr.x/R, vertexAttr.y/R, vertexAttr.z/R, 1.0/R);

    mat4 PMTt = transpose(MVPM * T); //mat4 PMTt = transpose(gl_ModelViewProjectionMatrix * T);

    vec4 r1 = PMTt[0];
    vec4 r2 = PMTt[1];
    vec4 r4 = PMTt[3];
    float r1Dr4T = dot(r1.xyz,r4.xyz)-r1.w*r4.w;
    float r1Dr1T = dot(r1.xyz,r1.xyz)-r1.w*r1.w;
    float r4Dr4T = dot(r4.xyz,r4.xyz)-r4.w*r4.w;
    float r2Dr2T = dot(r2.xyz,r2.xyz)-r2.w*r2.w;
    float r2Dr4T = dot(r2.xyz,r4.xyz)-r2.w*r4.w;

    //gl_Position = vec4(-r1Dr4T, -r2Dr4T, gl_Position.z/gl_Position.w*(-r4Dr4T), -r4Dr4T);

    float discriminant_x = r1Dr4T*r1Dr4T-r4Dr4T*r1Dr1T;
    float discriminant_y = r2Dr4T*r2Dr4T-r4Dr4T*r2Dr2T;
    float screen = max(float(viewport.z), float(viewport.w));

    gl_PointSize = sqrt(max(discriminant_x, discriminant_y))*screen/(-r4Dr4T);

    /*/ защита роста сферы от системных ограничений
    float newR=R;
    if (gl_PointSize>maxpointsize)
        {
        float factor = maxpointsize/gl_PointSize;
        newR=R*factor;
    }//*/

    radius=gl_PointSize/2;
    position=gl_Position;
}
