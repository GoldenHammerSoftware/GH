#include "ghdefines.glsl"
#include "reflectioninclude.glsl"
#include "ghlighting.glsl"

#ifdef SHADER_SUPPORTS_REFLECTION
varying highp vec4 v_reflectionPos;
uniform sampler2D ReflectionTexture;
uniform mediump float ReflectionAmt;
#endif

#ifdef SHADER_SUPPORTS_DETAIL_TEXTURE
varying mediump vec2 v_uvdetail;
uniform sampler2D DetailTexture;
#endif

varying mediump vec2 v_uvpbr;

#ifdef SHADER_PBR_UE4
uniform sampler2D SpecularTexture;
#endif

#ifdef SHADER_SUPPORTS_LIGHTMAP
varying mediump vec2 v_uvlightmap;
uniform sampler2D LightmapTexture;
uniform mediump float LightMapScale;
#endif

#ifdef SHADER_SUPPORTS_BUMPMAP
uniform sampler2D NormalTexture;
varying mediump vec3 v_tangent;
varying mediump vec3 v_binormal;
#endif

#ifdef SHADER_SUPPORTS_EMISSIVEMAP
uniform sampler2D EmissiveTexture;
#endif 

#if defined(SHADER_SUPPORTS_ENVIRONMENTMAP) || defined(SHADER_SUPPORTS_IBL)
uniform samplerCube EnvironmentTexture;
uniform highp mat4 ModelToWorld;
#endif

#ifdef SHADER_SUPPORTS_SHADOW
uniform sampler2D ShadowTexture;
varying highp vec4 v_shadowProjPos;
varying highp vec4 v_worldPos;
uniform mediump vec3 ShadowCenter;
uniform mediump float ShadowRadius;
#endif

varying highp vec2 v_uv;
varying highp vec3 v_normal;
varying highp vec4 v_modelPos;

uniform sampler2D DiffuseTexture;
uniform sampler2D MetalTexture;

uniform mediump float MaterialAmbient; 
uniform mediump float MaterialEmissive;
uniform mediump float LightAmbientAmount;
#if defined(SHADER_SUPPORTS_ENVIRONMENTMAP) || defined(SHADER_SUPPORTS_IBL)
uniform mediump float EnvironmentMapIntensity;
#endif
uniform highp vec3 EyePosModel;

uniform mediump vec3 LightDir0;
uniform mediump vec3 LightIntensity0;
#if (kMaxLights > 1)
uniform mediump vec3 LightDir1;
uniform mediump vec3 LightIntensity1;
#endif

#if defined(SHADER_SUPPORTS_TINT) || defined(SHADER_SUPPORTS_ENT_COLOR_MASK)
	uniform mediump vec4 EntColor;
	#ifdef SHADER_SUPPORTS_ENT_COLOR_MASK
		uniform mediump vec4 EntColor2;
	#endif
#endif


void main()
{
	mediump vec4 texelColor = texture2D(DiffuseTexture, v_uv);
	mediump vec4 metalColor = texture2D(MetalTexture, v_uvpbr);

#ifdef SHADER_SUPPORTS_TINT
	texelColor *= EntColor;
#else 
	#ifdef SHADER_SUPPORTS_ENT_COLOR_MASK
		mediump vec4 entColorBlend = (texelColor.r * EntColor) + ((1.0 - texelColor.r) * EntColor2);
		texelColor = entColorBlend * (1.0 - texelColor.b);
	#endif
#endif

#ifdef SIMPLE_RENDER
	return texelColor;
#endif

#ifdef SHADER_SUPPORTS_DETAIL_TEXTURE
	mediump vec4 detailColor = texture2D(DetailTexture, v_uvdetail);

	#ifdef SHADER_DETAIL_DARKEN
		texelColor.xyz *= mix(detailColor.xyz, vec3(1.0, 1.0, 1.0), (1.0 - detailColor.w));
	#else
		texelColor.xyz = mix(texelColor.xyz, detailColor.xyz, detailColor.w);
	#endif
#endif

#ifdef SHADER_PBR_UE4
	mediump vec4 specularColor = texture2D(SpecularTexture, v_uv);

	mediump float occlusion = metalColor.r;
	mediump float roughness = metalColor.g;
	mediump float metallic = metalColor.b;
	mediump float specularMult = specularColor.r;
#else
	mediump float metallic = metalColor.r;
	mediump float roughness = (1.0 - metalColor.g);
	mediump float specularMult = metalColor.b;
	mediump float occlusion = 1.0;
#endif

	// hack to avoid bad specular
	roughness = max(roughness, 0.4);

	mediump float lightAmbient = LightAmbientAmount * MaterialAmbient;

#ifdef SHADER_SUPPORTS_EMISSIVEMAP
#else
	lightAmbient = clamp(lightAmbient + MaterialEmissive, 0.0, 1.0);
#endif

#if defined(SHADER_SUPPORTS_ENVIRONMENTMAP) || defined(SHADER_SUPPORTS_IBL)
	mediump vec3 viewVecModel = (v_modelPos.xyz - EyePosModel);
#endif

#ifdef SHADER_SUPPORTS_BUMPMAP
	mediump vec4 normalColor = texture2D(NormalTexture, v_uv);
	highp vec3 bumpNormal = applyBumpmapNormal(normalColor.xyz, v_normal, v_tangent, v_binormal);
#else
	highp vec3 bumpNormal = v_normal;
#endif
	bumpNormal = normalize(bumpNormal);

	GHLight lights[kMaxLights];
	lights[0].mVec = LightDir0;
	lights[0].mIntensity = LightIntensity0;
#if (kMaxLights > 1)
	lights[1].mVec = LightDir1;
	lights[1].mIntensity = LightIntensity1;
#endif

	mediump float lightIntensityMod = 1.0;

#if defined(SHADER_SUPPORTS_LIGHTMAP) || ( (!defined(SHADER_QUALITY) || SHADER_QUALITY > 0) && defined(SHADER_SUPPORTS_SHADOW) )

#ifdef SHADER_SUPPORTS_LIGHTMAP
	mediump vec4 lightmapColor = texture2D(LightmapTexture, v_uvlightmap);
	lightmapColor.xyz = mix(vec3(1.0,1.0,1.0), lightmapColor.xyz, LightMapScale);
	lightIntensityMod *= lightmapColor.r;
#endif // lightmap

#if !defined(SHADER_QUALITY) || SHADER_QUALITY > 0
#ifdef SHADER_SUPPORTS_SHADOW
	mediump float shadowPct = getShadowAmount(v_shadowProjPos, ShadowTexture, v_worldPos, ShadowCenter, ShadowRadius);
	lightIntensityMod *= (1.0-shadowPct*0.1);
#endif // shadow
#endif // shader_quality

#endif // lm or shadow.

	highp vec3 eyeDir = normalize(EyePosModel - v_modelPos.xyz);

#if defined(SHADER_SUPPORTS_ENVIRONMENTMAP) || defined(SHADER_SUPPORTS_IBL)

    //Explicit mip definitions are based on 1024x1024 base cubemaps.
    // We may need to change these numbers if we change the cubemap resolution
    // These numbers were set by eyeballing for what "looks cool" in the TSB bowling alley.
    // Both the numbers and the formulas that use them are open to being changed.
    #ifdef GH_IOS
        const mediump float diffuseEnvMip = 8.0;
        const mediump float maxSpecularEnvMip = 3.0;
        const mediump float minSpecularEnvMip = 0.0;
    #else // assuming Android. Possible todo: add more platform defines
        const mediump float diffuseEnvMip = 4.0;
        const mediump float maxSpecularEnvMip = 3.0;
        const mediump float minSpecularEnvMip = 0.0;
    #endif
    
	mediump vec3 diffuseEnvUVW = normalize((ModelToWorld * vec4(bumpNormal, 0.0)).xyz);
	diffuseEnvUVW.x = -diffuseEnvUVW.x;
	mediump vec3 diffuseEnv = ghTextureCubeLOD(EnvironmentTexture, diffuseEnvUVW, diffuseEnvMip).xyz;

	mediump vec3 specEnvUVW = normalize(reflect(viewVecModel, bumpNormal));
	specEnvUVW = normalize((ModelToWorld * vec4(specEnvUVW, 0.0)).xyz);
	specEnvUVW.x = -specEnvUVW.x;

	mediump float specularEnvMip = minSpecularEnvMip + (maxSpecularEnvMip - minSpecularEnvMip)*log2(roughness + 1.0);
	mediump vec3 specularEnv = ghTextureCubeLOD(EnvironmentTexture, specEnvUVW, specularEnvMip).xyz;

#ifdef SHADER_SUPPORTS_ENVIRONMENTMAP
	// Make the reflection have less of an impact on diffuse.
	mediump vec3 diffuseAmbient = mix(vec3(1.0, 1.0, 1.0), diffuseEnv, 0.1);
#else
	mediump vec3 diffuseAmbient = mix(vec3(0.0, 0.0, 0.0), diffuseEnv, 0.5);
#endif
	mediump vec3 specularAmbient = specularEnv;

	mediump vec3 reflectionColor = cookTorranceOmniDirectional(texelColor.xyz, bumpNormal, metallic, 
		roughness, specularMult, diffuseAmbient, specularAmbient, eyeDir);

#ifdef SHADER_SUPPORTS_ENVIRONMENTMAP
	texelColor.xyz = mix(texelColor.xyz, reflectionColor, EnvironmentMapIntensity);
#endif

#endif // envmap

	// Don't show reflection on ovr go.  Search ifdef GH_OVR_GO in GHBowlingApp.cpp to re-enable.
#if !defined(SHADER_QUALITY) || SHADER_QUALITY > 0

#ifdef SHADER_SUPPORTS_REFLECTION
	texelColor = applyPixelReflection(v_reflectionPos, ReflectionTexture, ReflectionAmt*(1.0-roughness), texelColor);
#endif

#endif // SHADER_QUALITY

	lightIntensityMod *= occlusion;
	mediump vec3 lightAmbientColor = vec3(lightAmbient, lightAmbient, lightAmbient);

#ifdef SHADER_SUPPORTS_EMISSIVEMAP
	mediump vec3 litColor = applyLights(texelColor.xyz, bumpNormal, metallic, roughness, specularMult, lights, lightAmbientColor, lightAmbientColor, eyeDir, lightIntensityMod);

	mediump vec4 emissiveColor = texture2D(EmissiveTexture, v_uv);
	texelColor.xyz = mix(litColor.xyz, texelColor.xyz, emissiveColor.r + MaterialEmissive);
#else
	texelColor.xyz = applyLights(texelColor.xyz, bumpNormal, metallic, roughness, specularMult, lights, lightAmbientColor, lightAmbientColor, eyeDir, lightIntensityMod);
#endif

#ifdef SHADER_SUPPORTS_IBL
	texelColor.xyz += reflectionColor;
#endif

    gl_FragColor = texelColor;
}
