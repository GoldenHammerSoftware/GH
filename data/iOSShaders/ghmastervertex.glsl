#include "reflectioninclude.glsl"
// if would be nice if we could not include reflectioninclude.h if SHADER_SUPPORTS_REFLECTION isn't set but our preprocessor is not smart.

#ifdef SHADER_SUPPORTS_REFLECTION
uniform mat4 ReflectionViewProj;
varying highp vec4 v_reflectionPos;
#endif

#ifdef SHADER_SUPPORTS_DETAIL_TEXTURE
uniform vec2 DetailUVScale;
varying mediump vec2 v_uvdetail;
#endif

uniform mediump float PBRUsesDetailUV;
varying mediump vec2 v_uvpbr;

#ifdef SHADER_SUPPORTS_LIGHTMAP
varying mediump vec2 v_uvlightmap;
#endif
#if defined(SHADER_SUPPORTS_LIGHTMAP) || defined(SHADER_DETAIL_UV_CHANNEL)
	attribute mediump vec2 uv2;
#endif

#ifdef SHADER_SUPPORTS_BUMPMAP
attribute mediump vec3 tangent;
varying mediump vec3 v_tangent;
varying mediump vec3 v_binormal;
#endif

uniform mediump mat4 ViewProj;
uniform mediump vec2 DiffuseUVScale;
uniform highp mat4 ModelToWorld;

attribute highp vec4 position;
attribute mediump vec3 normal;
attribute mediump vec2 uv1;

varying highp vec2 v_uv;
varying highp vec3 v_normal;
varying highp vec4 v_modelPos;

#ifdef SHADER_SUPPORTS_SHADOW
varying highp vec4 v_shadowProjPos;
uniform mat4 ShadowViewProj;
varying highp vec4 v_worldPos;
#endif

void main()
{
	v_modelPos = position;
	highp vec4 worldPos = ModelToWorld * position;
	gl_Position = ViewProj * worldPos;

#if !defined(SHADER_QUALITY) || SHADER_QUALITY > 0
#ifdef SHADER_SUPPORTS_SHADOW
	v_shadowProjPos = calcVertexReflection(ShadowViewProj, worldPos);
	v_worldPos = worldPos;
#endif
#endif

#ifdef SHADER_SUPPORTS_REFLECTION
	#if !defined(SHADER_QUALITY) || SHADER_QUALITY > 0
		v_reflectionPos = calcVertexReflection(ReflectionViewProj, worldPos);
	#else
		v_reflectionPos = vec4(0.0,0.0,0.0,0.0);
	#endif // SHADER_QUALITY 
#endif // SHADER_SUPPORTS_REFLECTION

	v_uv = uv1 * DiffuseUVScale;
#ifdef SHADER_SUPPORTS_LIGHTMAP
	v_uvlightmap = uv2;
#endif

#ifdef SHADER_SUPPORTS_DETAIL_TEXTURE
	#if defined(SHADER_DETAIL_UV_CHANNEL) && (SHADER_DETAIL_UV_CHANNEL == 1)
		v_uvdetail = uv2;
		mediump vec2 centerOffset = vec2(0.5, 0.5);
		v_uvdetail -= centerOffset;
		v_uvdetail *= DetailUVScale;
		v_uvdetail += centerOffset;
	#else
		v_uvdetail = uv1 * DetailUVScale;
	#endif

	// decide whether the pbr textures will use the base or detail uv channel.
	v_uvpbr = mix(v_uv, v_uvdetail, PBRUsesDetailUV);
#else
	v_uvpbr = v_uv;

#endif

	v_normal = normal;
#ifdef SHADER_SUPPORTS_BUMPMAP
	v_tangent = normalize(tangent);
	v_binormal = cross(normal, v_tangent);
#endif
}
