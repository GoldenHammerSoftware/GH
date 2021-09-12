uniform mat4 ViewProj;
uniform mat4 ModelToWorld;
uniform mat4 WorldToModel;
uniform vec3 EyePos;
uniform float TextureWorldSize;

uniform mat4 ShadowViewProj;
uniform mat4 WideShadowViewProj;
uniform mat4 TrailViewProj;

attribute vec4 position;

varying vec2 v_uv1;
varying vec2 v_grounduv;
varying vec3 v_cliffuv;
varying vec4 v_TrailPos;
varying vec4 v_ShadowPos;
varying vec4 v_WideShadowPos;
varying vec3 v_modelEyeDir;

void main()
{
    vec4 worldPos = ModelToWorld * position;
    gl_Position = ViewProj * worldPos;

    v_uv1.x = clamp(position.x / TextureWorldSize, 0.0, 1.0);
    v_uv1.y = clamp(position.z / TextureWorldSize, 0.0, 1.0);
    v_uv1.y = 1.0 - v_uv1.y;
    
    v_grounduv = worldPos.xz * 1.0;
    
    v_cliffuv.x = v_uv1.x;
    v_cliffuv.y = clamp(position.z / TextureWorldSize, 0.0, 1.0);
    v_cliffuv.z = v_uv1.y;
    v_cliffuv *= 200.0;
    
    v_ShadowPos = ShadowViewProj * worldPos;
    v_ShadowPos.xy /= 2.0;
    v_ShadowPos.xy += v_ShadowPos.w/2.0;

    v_WideShadowPos = WideShadowViewProj * worldPos;
    v_WideShadowPos.xy /= 2.0;
    v_WideShadowPos.xy += v_WideShadowPos.w/2.0;

    v_TrailPos = TrailViewProj * worldPos;
    v_TrailPos.xy /= 2.0;
    v_TrailPos.xy += v_TrailPos.w/2.0;
    
    vec4 eyePos4;
	eyePos4.xyz = EyePos.xyz;
	eyePos4.w = 1.0;
	vec4 modelEyePos = WorldToModel * eyePos4;
	v_modelEyeDir = normalize(position.xyz - modelEyePos.xyz);
}

