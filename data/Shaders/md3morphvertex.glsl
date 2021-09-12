uniform mat4 ModelViewProj;
uniform float InterpPct;

attribute vec4 position;
attribute vec2 uv1;
attribute vec4 diffuse;

//attribute vec3 normal;
//attribute vec3 specular;

varying vec2 v_uv1;

void main()
{
    vec4 interpolatedPos = mix(position, diffuse, InterpPct);

    gl_Position = ModelViewProj * interpolatedPos;
    v_uv1 = uv1;
}
