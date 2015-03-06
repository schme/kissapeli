#version 330

in vec3 Color;
in vec2 Texcoord;
out vec4 outColor;
uniform sampler2D p1score;
uniform sampler2D p2score;

void main() {

    /*outColor = vec4(texture(p1score, Texcoord).a, texture(p2score, Texcoord).a,*/
                /*0.0, 1.0);*/
    outColor = vec4(1);
}
