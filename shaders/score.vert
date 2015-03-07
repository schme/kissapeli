#version 330

in vec2 position;
in vec3 color;
in vec2 texcoord;

out vec3 Color;
out vec2 Texcoord;

uniform vec2 screenSize;
uniform vec2 boardSize;

void main() {
    Color = color;
    Texcoord = texcoord;

    vec2 shiftPos = position / boardSize * screenSize;
    shiftPos -= screenSize/2.0;

    mat4 m = mat4(
        2.0 / screenSize.x, 0.0, 0.0, 0.0,
        0.0, 2.0 / screenSize.y, 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0,
        0.0, 0.0, 0.0, 1.0);

    gl_Position = m * vec4( shiftPos, 0.0, 1.0);
}
