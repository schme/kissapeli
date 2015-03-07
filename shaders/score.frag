#version 330

in vec3 Color;
in vec2 Texcoord;
out vec4 outColor;

uniform int score;
uniform sampler2D p1score;
uniform sampler2D p2score;

void main() {

    if( score == 1) {
        outColor = vec4( Color, texture( p1score, Texcoord).a);
    } else {
        outColor = vec4( Color, texture( p2score, Texcoord).a);
    }
}
