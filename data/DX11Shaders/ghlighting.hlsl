#ifndef _GHLIGHTING_
#define _GHLIGHTING_

#define kMaxLights 2

struct GHLight
{
	// direction for directional lights.
	// direction from light to object for point light.
	float3 mVec;
	// how strong is this light and what color?
	float3 mIntensity;
};

float calcBaseLight(float3 lightDir, float lightAmbient, float3 normal, float materialAmbient)
{
	float ambientAmount = lightAmbient * (1.0 - materialAmbient);
	float ret = max(dot(lightDir, normal), 0.0);
	ret += ambientAmount;
	return min(ret, 1.0);
}

float calcSpecular(float3 eyePosWorld, float3 lightDir, float3 pos, float3 normal, float specularAmount, float specularPower, float4x4 worldToModel)
{
	float4 modelEyePos = mul(worldToModel, float4(eyePosWorld.x, eyePosWorld.y, eyePosWorld.z, 1.0));
	float3 eyeDir = normalize(pos.xyz - modelEyePos.xyz);
	float3 eyeReflect = reflect(eyeDir, normal);
	float specular = pow(max(0.0, dot(eyeReflect, lightDir)), specularPower);
	specular *= specularAmount;
	return specular;
}

void calcTangentBinormal(float3 normal, inout float3 tangent, inout float3 binormal)
{
	// http://www.ozone3d.net/smf/index.php?topic=80.0
	float3 c1 = cross(normal, float3(0.0,0.0,1.0));
	float3 c2 = cross(normal, float3(0.0,1.0,0.0));

	if (length(c1) > length(c2))
	{
		tangent = c1;
	}
	else
	{
		tangent = c2;
	}

	tangent = normalize(tangent);
	binormal = cross(tangent, normal);
	binormal = normalize(binormal);
}

// http://www.thetenthplanet.de/archives/1180
float3x3 cotangent_frame(float3 N, float3 p, float2 uv)
{
	// get edge vectors of the pixel triangle
	float3 dp1 = ddx(p);
	float3 dp2 = ddy(p);
	float2 duv1 = ddx(uv);
	float2 duv2 = ddy(uv);

	// solve the linear system
	float3 dp2perp = cross(dp2, N);
	float3 dp1perp = cross(N, dp1);
	float3 T = dp2perp * duv1.x + dp1perp * duv2.x;
	float3 B = dp2perp * duv1.y + dp1perp * duv2.y;

	// construct a scale-invariant frame 
	float invmax = rsqrt(max(dot(T, T), dot(B, B)));
	return float3x3(T * invmax, B * invmax, N);
}

// http://www.thetenthplanet.de/archives/1180
float3 applyBumpmapNormal(float3 normColor, float3 modelNormal, float3 viewVec, float2 uv)
{
	// assume N, the interpolated vertex normal and 
	// V, the view vector (vertex to eye)
	float3x3 TBN = cotangent_frame(modelNormal, viewVec, uv);
	float3 bump = 2.0 * normColor - 1.0;
	bump = bump.yxz;
	bump.g *= -1.0;
	bump = normalize(mul(bump, TBN)).xyz;
	return bump;
}

float3 applyBumpmapNormal(float3 normColor, float3 modelNormal, float3 tangent, float3 binormal)
{
	// https://stackoverflow.com/questions/14972189/implementing-normal-mapping-hlsl
	// http://rbwhitaker.wikidot.com/bump-map-shader-2

	float3 nTangent = normalize(tangent);
	float3 nBinormal = normalize(binormal);
	float3 nNormal = normalize(modelNormal);

	float3 bump = 2.0 * normColor.xyz - 1.0;
	bump = bump.yxz;
	bump.x *= -1.0;

	float3x3 TBN = float3x3( nTangent, nBinormal, nNormal );
	float3 bumpNormal = mul(bump, TBN);

	return bumpNormal;
}

// https://github.com/Nadrin/PBR/blob/master/data/shaders/hlsl/pbr.hlsl
// GGX/Towbridge-Reitz normal distribution function.
float ndfGGX(float cosLh, float roughness)
{
	float alpha = roughness * roughness;
	float alphaSq = alpha * alpha;

	float denom = (cosLh * cosLh) * (alphaSq - 1.0) + 1.0;
	float PI = 3.141592;
	return alphaSq / (PI * denom * denom);
}

// https://github.com/Nadrin/PBR/blob/master/data/shaders/hlsl/pbr.hlsl
// Single term for separable Schlick-GGX below.
float gaSchlickG1(float cosTheta, float k)
{
	return cosTheta / (cosTheta * (1.0 - k) + k);
}

// https://github.com/Nadrin/PBR/blob/master/data/shaders/hlsl/pbr.hlsl
// Schlick-GGX approximation of geometric attenuation function using Smith's method.
float gaSchlickGGX(float cosLi, float cosLo, float roughness)
{
	float r = roughness + 1.0;
	float k = (r * r) / 8.0; // Epic suggests using this roughness remapping for analytic lights.
	return gaSchlickG1(cosLi, k) * gaSchlickG1(cosLo, k);
}

float3 fresnelSchlick(float3 F0, float cosTheta)
{
	return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

float3 calcLightContribution(float3 lightDir, float3 eyeDir, float3 diffuseLuminance, float3 specularLuminance, float3 normal, 
							 float roughness, float cosEye, float metallic, float3 diffuse, float matSpecular)
{
	// Constant normal incidence Fresnel factor for all non-metal.
	float3 Fdielectric = 0.2;
	float noDiv0 = 1e-5f;

	float nDotL = max(dot(normal, lightDir), 0.0);

	float3 halfVector = normalize(lightDir + eyeDir);
	float3 radiance = 1.0;
	float cosLight = max(0.0, dot(normal, lightDir));
	float cosHalf = max(0.0, dot(normal, halfVector));

	float3 F0 = lerp(Fdielectric, max(float3(0.1, 0.1, 0.1), diffuse), metallic);
	float3 F = fresnelSchlick(F0, max(0, dot(halfVector, eyeDir)));
	F *= matSpecular;
	float D = ndfGGX(cosHalf, roughness);
	float G = gaSchlickGGX(cosLight, cosEye, roughness);

	// metal has no diffuse contribution
	float3 kd = lerp(float3(1, 1, 1) - F, float3(0, 0, 0), metallic);
	float3 diffuseBRDF = kd * diffuse;
	float3 specularBRDF = (F * D * G) / max(noDiv0, 4.0 * cosLight * cosEye);

	float3 lightContrib = (diffuseBRDF * diffuseLuminance + specularBRDF * specularLuminance) * cosLight;
	return lightContrib;
}

float3 cookTorranceOmniDirectional(float3 diffuse, float3 normal, float metallic, float roughness, float matSpecular,
	float3 diffuseLight, float3 specularLight, float3 eyeDir)
{
	// http://ruh.li/GraphicsCookTorrance.html
	// https://renderman.pixar.com/view/cook-torrance-surface-shader-code
	// https://www.gamedev.net/forums/topic/444098-hlsl-cook-torrance-lighting/
	// https://github.com/Nadrin/PBR/blob/master/data/shaders/hlsl/pbr.hlsl

	// lightDir should already be normalized in GHLightMaterialCallback.
	float cosEye = max(0.0, dot(normal, eyeDir));

	// make a fake light for ambient that points directly at the normal.
	float3 ambLightDir = normal;
	float ambRoughness = roughness;
	float ambMetal = saturate(metallic - 0.5); // cheat to get more of the diffuse in there.
	float3 finalValue = calcLightContribution(ambLightDir, eyeDir, diffuseLight, specularLight, normal, roughness, cosEye, ambMetal, diffuse, matSpecular);

	return finalValue;
}

float3 cookTorrance(float3 diffuse, float3 normal, float metallic, float roughness, float matSpecular,
					GHLight lights[kMaxLights], float ambient, float lightIntensityMod, float3 eyeDir)
{
	// http://ruh.li/GraphicsCookTorrance.html
	// https://renderman.pixar.com/view/cook-torrance-surface-shader-code
	// https://www.gamedev.net/forums/topic/444098-hlsl-cook-torrance-lighting/
	// https://github.com/Nadrin/PBR/blob/master/data/shaders/hlsl/pbr.hlsl

	float3 finalValue = float3(0.0, 0.0, 0.0);

#if !defined(SHADER_QUALITY) || SHADER_QUALITY > 0

	// apply the occlusion to the lights.
	ambient *= lightIntensityMod;

	// lightDir should already be normalized in GHLightMaterialCallback.
	float cosEye = max(0.0, dot(normal, eyeDir));
	for (int i = 0; i < kMaxLights; ++i)
	{
		lights[0].mIntensity *= lightIntensityMod;
		float3 lightDir = lights[i].mVec;
		float3 luminance = lights[i].mIntensity;

		float3 lightContrib = calcLightContribution(lightDir, eyeDir, luminance, luminance, normal, roughness, cosEye, metallic, diffuse, matSpecular);
		finalValue += lightContrib;
	}
#else
	ambient = 1.0 * lightIntensityMod;
#endif

	finalValue += cookTorranceOmniDirectional(diffuse, normal, metallic, roughness, matSpecular, ambient, ambient, eyeDir);

	return finalValue;
}

#endif
