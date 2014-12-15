#version 440
out vec4 color;
flat in vec4 objColor;

void main()
{
    vec4 newColor = objColor;

    // player 1
    if( objColor == vec4( 1.0, 0.0, 0.0, 1.0) ) {

    // player 2
    } else if( objColor == vec4( 0.0, 1.0, 0.0, 1.0) ) {

    // ball
    } else if( objColor == vec4( 0.0, 0.0, 1.0, 1.0) ) {

    // background
    } else if( objColor == vec4( 0.1, 0.1, 0.1, 0.1) ) {

    }

    color = newColor;
}
