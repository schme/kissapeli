#version 440
out vec4 color;
flat in vec4 objColor;

uniform vec2 screenSize;
uniform int frame;

vec4 ballColor = vec4(1.0, 0.2, 1.0, 1.0);
vec4 player1Color = vec4(0.9, 0.3, 0.5, 1.0);
vec4 player2Color = vec4(0.5, 0.3, 0.9, 1.0);
vec4 middleBarColor = vec4(0.7, 0.7, 0.9, 1.0);
vec4 bg_color = vec4(0.0, 0.0, 0.05, 1.0);

float FPS = 60.0;
float middleBarHalfWidth = 0.002;

void main()
{
    vec4 newColor = objColor;
    vec2 normMiddle = screenSize / 2.0 / screenSize;
    vec2 normCoord = gl_FragCoord.xy / screenSize;

    float dirtyTime = frame / FPS;  // lol

    /**
     * Different objects are passed in with different colors (objColor)
     * player1:     1,0,0,1
     * player2:     0,1,0,1
     * ball:        0,0,1,1
     * background:  .1,.1,.1,1
     */

    // player 1
    if( objColor == vec4( 1.0, 0.0, 0.0, 1.0) ) {
        newColor = player1Color;

    // player 2
    } else if( objColor == vec4( 0.0, 1.0, 0.0, 1.0) ) {
        newColor = player2Color;

    // ball
    } else if( objColor == vec4( 0.0, 0.0, 1.0, 1.0) ) {
        newColor = ballColor;

    // background
    } else if( objColor == vec4( 0.1, 0.1, 0.1, 1.0) ) {

        // middle bar
        if( normCoord.x <= normMiddle.x + middleBarHalfWidth &&
            normCoord.x >= normMiddle.x - middleBarHalfWidth) {

            newColor = middleBarColor;

        } else {

            vec4 wave_color = vec4(0.0);
            
            // Waves
            float wave_width = 0.01;
            normCoord  = -1.0 + 2.0 * normCoord;
            normCoord.y += 0.1;
            for(float i = 0.0; i < 2.0; i++) {
                    
                normCoord.y += (0.07 * sin(normCoord.x + i/10.0 + dirtyTime ));
                wave_width = abs(1.0 / (150.0 * normCoord.y));
                wave_color += vec4(wave_width * 1.0, wave_width, wave_width * 1.5, 1.0);
            }
            
            newColor = bg_color + wave_color;
        }
    }

    color = newColor;

}


/*
 * Wave effect modified from from bonniem's 'Waves' at www.shadertoy.com !
 */
