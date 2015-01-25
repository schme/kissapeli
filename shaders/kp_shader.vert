#version 440
layout(location = 0) in vec2 position;
layout(location = 1) in vec4 color;

flat out vec4 objColor;

uniform vec2 screenSize;
uniform vec2 boardSize;
uniform int frame;


void main() {
    objColor = color;

    vec2 shiftPos = position / boardSize * screenSize;
    shiftPos -= screenSize/2.0;

    mat4 m = mat4(
        2.0 / screenSize.x, 0.0, 0.0, 0.0,
        0.0, 2.0 / screenSize.y, 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0,
        0.0, 0.0, 0.0, 1.0);

    gl_Position = vec4( shiftPos, 0.0, 1.0) * m;

}
