#version 440
void main(void)
{
     vec2 pos = mod(gl_FragCoord.xy, vec2(50.0)) - vec2(25.0);
     float dist_squared = dot(pos, pos);
 
     if (dist_squared > 475.0)
             discard;
 
     gl_FragColor = mix(vec4(.90, .90, .90, 1.0), vec4(.20, .20, .40, 1.0),
                        smoothstep(280.25, 320.25, dist_squared));
}
