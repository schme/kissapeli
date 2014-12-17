#version 440
out vec4 color;
flat in vec4 objColor;

uniform vec2 screenSize;
uniform int frame;

vec4 ballColor = vec4(1.0, 0.3, 1.0, 1.0);
vec4 player1Color = vec4(0.7, 0.3, 0.7, 1.0);
vec4 player2Color = vec4(0.7, 0.3, 0.7, 1.0);
vec4 middleBarColor = vec4(1.0, 1.0, 1.0, 0.5);


vec3 COLOR1 = vec3(0.0, 0.0, 0.1);
vec3 COLOR2 = vec3(0.4, 0.3, 0.0);
float BLOCK_WIDTH = 0.01;

void main()
{
    vec4 newColor = objColor;
    // in pixels, x min, y max
    float middleBarHalfWidth = 4.0;
    vec2 middleBarHalfPos = vec2( screenSize.x / 2.f - middleBarHalfWidth,
                                  screenSize.x / 2.f + middleBarHalfWidth);
    float dirtyTime = frame / 60.0;

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
        if( gl_FragCoord.x >= middleBarHalfPos.x &&
            gl_FragCoord.x <= middleBarHalfPos.y ) {

            newColor = middleBarColor;

        } else {

            vec2 uv = gl_FragCoord.xy / screenSize.xy;
            
            // To create the BG pattern
            vec3 final_color = vec3(0.0);
            vec3 bg_color = vec3(1.0);
            vec3 wave_color = vec3(0.0);
            
            float c1 = mod(uv.x, 2.0 * BLOCK_WIDTH);
            c1 = step(BLOCK_WIDTH, c1);
            
            float c2 = mod(uv.y, 2.0 * BLOCK_WIDTH);
            c2 = step(BLOCK_WIDTH, c2);
            
            bg_color = mix(uv.x * COLOR1, uv.y * COLOR2, c1 * c2);
            
            
            // To create the waves
            float wave_width = 0.001;
            uv  = -1.0 + 2.0 * uv;
            uv.y += 0.1;
            for(float i = 0.0; i < 10.0; i++) {
                    
                    uv.y += (0.07 * sin(uv.x + i/7.0 + dirtyTime ));
                    wave_width = abs(1.0 / (150.0 * uv.y));
                    wave_color += vec3(wave_width * 1.0, wave_width, wave_width * 1.5);
            }
            
            final_color = bg_color + wave_color;
            newColor = vec4(final_color, 1.0);
        }

    }

    color = newColor;

}
