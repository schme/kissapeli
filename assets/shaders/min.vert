#version 440
layout(location = 0) in vec2 position;

flat out vec4 vertColor;

uniform vec2 screenSize;
uniform vec2 playIndex;

void main() {

    vec2 clipPos = position;
    clipPos -= screenSize/2.0;
    clipPos = clipPos / (screenSize/2.0);


    if( clipPos == vec2(-1.0,-1.0) ||
        clipPos == vec2(-1.0,1.0) ||
        clipPos == vec2(1.0,-1.0) ||
        clipPos == vec2(1.0,1.0)) {

        vertColor = vec4( 1.0, 1.0, 0.0, 1.0);
        gl_Position = vec4( clipPos, 0.7, 1.0);
    } else {

        vertColor = vec4( 0.2, 0.2, 0.2, 1.0);
        gl_Position = vec4( clipPos, 0.8, 1.0);
    }
}
