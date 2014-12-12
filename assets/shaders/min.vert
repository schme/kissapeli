#version 440
layout(location = 0) in vec2 position;

uniform vec2 screenSize;

void main() {

    vec2 clipPos = position;
    clipPos -= screenSize/2.0;
    clipPos = clipPos / (screenSize/2.0);
    
    gl_Position = vec4( clipPos, 0.8, 1.0);
}
