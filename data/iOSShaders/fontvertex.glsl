#include "guivertexinclude.glsl"

attribute mediump vec4 position;
attribute mediump vec2 uv1;
attribute mediump vec4 diffuse;
attribute mediump vec4 specular;

varying mediump vec2 v_uv1;
varying mediump vec4 v_diffuse;
varying mediump vec4 v_specular;

void main()
{
    gl_Position = positionGUIVertex(position.xy);
    v_uv1 = uv1;
    v_diffuse = diffuse;
    v_diffuse.xyz /= 255.0;
    v_specular = specular;
    v_specular.xyz /= 255.0;
}
