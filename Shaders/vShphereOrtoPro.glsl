#version 120
attribute highp vec4 vertexAttr;
uniform mediump mat4 MVPM;  // gl_ModelViewProjectionMatrix
attribute lowp vec4 colorAttr;
uniform float R;
uniform float maxpointsize;
uniform vec4 viewport; // //получаем размеры окна рисования (x0,y0,w,h)

// for perspective proection
varying mat4 VPMTInverse;
varying mat4 VPInverse;
varying vec3 centernormclip;

// это временный костыль
varying lowp vec4 color;
varying float radius;
varying vec3  center;
varying vec4  position;
// end kostyele

void main()
{
gl_Position = MVPM * vertexAttr; // первоначальное положение , затем будем трансформировать с учётом перспективного искажения шара
gl_FrontColor = colorAttr;
color = colorAttr;

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

    gl_Position = vec4(-r1Dr4T, -r2Dr4T, gl_Position.z/gl_Position.w*(-r4Dr4T), -r4Dr4T);
    // в ортогональной проекции он совпадает и не требует дополнительных преобразований

    float discriminant_x = r1Dr4T*r1Dr4T-r4Dr4T*r1Dr1T;
    float discriminant_y = r2Dr4T*r2Dr4T-r4Dr4T*r2Dr2T;
    float screen = max(float(viewport.z), float(viewport.w));

    //gl_PointSize = sqrt(max(discriminant_x, discriminant_y))*screen/(-r4Dr4T);
    gl_PointSize = viewport.w*2;

    // temp of костыль
    radius=viewport.w;//gl_PointSize;
    center=gl_Position.xyz;
    position=gl_Position;
}
