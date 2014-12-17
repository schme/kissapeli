#version 440
out vec4 color;
flat in vec4 objColor;

uniform vec2 screenSize;

void main()
{
    vec4 newColor = objColor;
    // in pixels, x min, y max
    float middleBarHalfWidth = 4.0;
    vec2 middleBarHalfPos = vec2( screenSize.x / 2.f - middleBarHalfWidth,
                                  screenSize.x / 2.f + middleBarHalfWidth);

    /**
     * Different objects are passed in with different colors (objColor)
     * player1:     1,0,0,1
     * player2:     0,1,0,1
     * ball:        0,0,1,1
     * background:  .1,.1,.1,1
     */

    // player 1
    if( objColor == vec4( 1.0, 0.0, 0.0, 1.0) ) {

    // player 2
    } else if( objColor == vec4( 0.0, 1.0, 0.0, 1.0) ) {

    // ball
    } else if( objColor == vec4( 0.0, 0.0, 1.0, 1.0) ) {

    // background
    } else if( objColor == vec4( 0.1, 0.1, 0.1, 1.0) ) {

        if( gl_FragCoord.x >= middleBarHalfPos.x &&
            gl_FragCoord.x <= middleBarHalfPos.y ) {

            newColor = vec4( 0.2, 0.2, 0.2, 1.0);
        }
    }

    color = newColor;
}
