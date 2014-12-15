#version 440
out vec4 color;
flat in vec4 vertColor;

uniform vec2 playIndex;
void main()
{
    float PI = 3.14159;

    vec4 newColor = vertColor;
    float ballSpinValue = 50.0;
    vec2 offset = vec2( sin( playIndex.x ) * 50.0 * PI, sin( playIndex.y ) * 10. * PI);
    vec2 normalizedOffset = vec2( sin( playIndex.x) * ballSpinValue, sin( playIndex.y / 2) * ballSpinValue);

    vec4 colorOffset = vec4( sin(offset.xy), offset.y, 1.0);
    vec2 ballgap = vec2(62.0);


    if( vertColor == vec4( 1.0, 1.0, 0.0, 1.0)) {

        vec2 pos = mod(gl_FragCoord.xy + normalizedOffset, ballgap) - ballgap/2.;
        float dist_squared = dot(pos, pos) + offset.x + offset.y;
        if (dist_squared > 375.0) {
            discard;
        } else {
            newColor = mix(colorOffset, vec4(.20, .20, .20, 1.0),
                                smoothstep(180.25, 220.25, dist_squared));
        }
    } else if( vertColor == vec4( 0.2, 0.2, 0.2, 1.0) ) {

    }

    color = newColor;
}
