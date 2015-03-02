#version 440

#define BACKGROUND 0
#define PLAYER1 1
#define PLAYER2 2
#define BALL 3

out vec4 color;
flat in vec4 vertColor;

uniform vec2 screenSize;
uniform vec2 boardSize;
uniform int frame;
uniform int object;


void main()
{
    color = vertColor;

    if( object == BACKGROUND) {

        const vec4 middleBarColor = vec4(0.7, 0.7, 0.9, 1.0);
        const float middleBarHalfWidth = 0.002;
        const float FPS = 60.0;


        vec2 normMiddle = screenSize / 2.0 / screenSize;
        vec2 normCoord = gl_FragCoord.xy / screenSize;

        float dirtyTime = frame / FPS;  // lol

        // middle bar
        if( normCoord.x <= normMiddle.x + middleBarHalfWidth &&
            normCoord.x >= normMiddle.x - middleBarHalfWidth) {

            color = middleBarColor;

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
            
            color += wave_color;
        }
    }

}


/*
 * Wave effect modified from from bonniem's 'Waves' at www.shadertoy.com !
 */
