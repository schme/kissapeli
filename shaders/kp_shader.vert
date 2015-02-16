#version 440

#define BACKGROUND 0
#define PLAYER1 1
#define PLAYER2 2
#define BALL 3

layout(location = 0) in vec2 position;
layout(location = 1) in vec4 color;

flat out vec4 vertColor;

uniform vec2 screenSize;
uniform vec2 boardSize;
uniform int frame;
uniform int object;


void main() {
    vertColor = color;

    vec2 shiftPos = position / boardSize * screenSize;
    shiftPos -= screenSize/2.0;

    const mat4 m = mat4(
        2.0 / screenSize.x, 0.0, 0.0, 0.0,
        0.0, 2.0 / screenSize.y, 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0,
        0.0, 0.0, 0.0, 1.0);

    gl_Position = m * vec4( shiftPos, 0.0, 1.0);

}
