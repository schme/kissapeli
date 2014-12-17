#version 440
layout(location = 0) in vec2 position;
layout(location = 1) in vec4 color;

flat out vec4 objColor;

uniform vec2 screenSize;

void main() {
    objColor = color;

    vec2 clipPos = position;
    clipPos -= screenSize/2.0;
    clipPos = clipPos / (screenSize/2.0);



    gl_Position = vec4( clipPos, 1.0, 1.0);
}
