attribute vec4 position;
attribute vec2 uv1;
attribute vec4 diffuse;
attribute vec4 specular;

mat4 orthoProj = mat4(	2, 0, 0, 0,
						0, -2, 0, 0,
						0, 0, -1, 0,
						-1, 1, 0, 1 );

varying vec2 v_uv1;
varying vec4 v_diffuse;
varying vec4 v_specular;

void main()
{
    gl_Position = orthoProj * position;
    v_uv1 = uv1;
    v_diffuse = diffuse;
    v_diffuse.xyz /= 255.0;
    v_specular = specular;
    v_specular.xyz /= 255.0;
}
