varying highp vec2 texCoord;  // входная переменная (переданная из вершинного шейдера)
uniform sampler2D texUniform;

void main () {
    gl_FragColor = texture2D(texUniform, texCoord); //
}
