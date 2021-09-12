#define kMaxLights 2

struct GHLight
{
	mediump vec3 mVec;
	lowp vec3 mIntensity;
};

lowp float calcBaseLight(mediump vec3 lightDir, lowp float lightAmbient, mediump vec3 normal, lowp float materialAmbient)
{
	lowp float ambientAmount = lightAmbient * (1.0 - materialAmbient);
	lowp float ret = max(dot(lightDir, normal), 0.0);
	ret += ambientAmount;
	return min(ret, 1.0);
}

mediump float calcSpecular(mediump vec3 eyePosWorld, mediump vec3 lightDir, mediump vec3 pos, mediump vec3 normal, mediump float specularAmount, mediump float specularPower, mediump mat4 worldToModel)
{
	highp vec4 modelEyePos = worldToModel * vec4(eyePosWorld.x, eyePosWorld.y, eyePosWorld.z, 1.0);
	highp vec3 eyeDir = normalize(pos.xyz - modelEyePos.xyz);
	highp vec3 eyeReflect = reflect(eyeDir, normal);
	highp float specular = pow(max(0.0, dot(eyeReflect, lightDir)), specularPower);
	specular *= specularAmount;
	return specular;
}

mediump vec3 applyBumpmapNormal(mediump vec3 normColor, mediump vec3 modelNormal, mediump vec3 tangent, mediump vec3 binormal)
{
	// https://stackoverflow.com/questions/14972189/implementing-normal-mapping-hlsl
	// http://rbwhitaker.wikidot.com/bump-map-shader-2

	mediump vec3 nTangent = normalize(tangent);
	mediump vec3 nBinormal = normalize(binormal);
	mediump vec3 nNormal = normalize(modelNormal);

	mediump vec3 bump = 2.0 * normColor.xyz - 1.0;
	bump = bump.yxz;
	bump.r *= -1.0;

	mediump mat3 TBN = mat3( nTangent, nBinormal, nNormal );
	mediump vec3 bumpNormal = TBN * bump;

	return bumpNormal;
}

void calcTangentBinormal(mediump vec3 normal, inout mediump vec3 tangent, inout mediump vec3 binormal)
{
	// http://www.ozone3d.net/smf/index.php?topic=80.0
	mediump vec3 c1 = cross(normal, vec3(0.0,0.0,1.0));
	mediump vec3 c2 = cross(normal, vec3(0.0,1.0,0.0));

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

#ifdef GL_OES_standard_derivatives
// dFdx/dFdy requires GL_OES_standard_derivatives.

// http://www.thetenthplanet.de/archives/1180
mediump mat3 cotangent_frame( mediump vec3 N, mediump vec3 p, mediump vec2 uv )
{
	// todo: I'm not sure if these all need to be highp.
	// default precision was not enough.

    // get edge vectors of the pixel triangle
    mediump vec3 dp1 = dFdx( p );
    mediump vec3 dp2 = dFdy( p );
    mediump vec2 duv1 = dFdx( uv );
    mediump vec2 duv2 = dFdy( uv );
 
    // solve the linear system
    mediump vec3 dp2perp = cross( dp2, N );
    mediump vec3 dp1perp = cross( N, dp1 );
    mediump vec3 T = dp2perp * duv1.x + dp1perp * duv2.x;
    mediump vec3 B = dp2perp * duv1.y + dp1perp * duv2.y;
 
    // construct a scale-invariant frame 
    mediump float invmax = inversesqrt( max( dot(T,T), dot(B,B) ) );
    return mat3( T * invmax, B * invmax, N );
}
#endif

// http://www.thetenthplanet.de/archives/1180
mediump vec3 applyBumpmapNormal(mediump vec3 normColor, mediump vec3 modelNormal, mediump vec3 viewVec, mediump vec2 uv)
{
#ifdef GL_OES_standard_derivatives
	// decode the normal.
	mediump vec3 bump = 2.0 * normColor - 1.0;
	bump = bump.yxz;
	bump.g *= -1.0;

	mediump mat3 TBN = cotangent_frame(modelNormal, -viewVec, uv);
    return normalize( TBN * bump );
#else
	// if dydx is unsupported turn off bump mapping.
	return modelNormal;
#endif
}

// https://github.com/Nadrin/PBR/blob/master/data/shaders/hlsl/pbr.hlsl
// GGX/Towbridge-Reitz normal distribution function.
highp float ndfGGX(highp float cosLh, highp float roughness)
{
	highp float alpha = roughness * roughness;
	highp float alphaSq = alpha * alpha;

	highp float denom = (cosLh * cosLh) * (alphaSq - 1.0) + 1.0;
	highp float PI = 3.141592;
	return alphaSq / (PI * denom * denom);
}

// https://github.com/Nadrin/PBR/blob/master/data/shaders/hlsl/pbr.hlsl
// Single term for separable Schlick-GGX below.
highp float gaSchlickG1(highp float cosTheta, highp float k)
{
	return cosTheta / (cosTheta * (1.0 - k) + k);
}

// https://github.com/Nadrin/PBR/blob/master/data/shaders/hlsl/pbr.hlsl
// Schlick-GGX approximation of geometric attenuation function using Smith's method.
highp float gaSchlickGGX(highp float cosLi, highp float cosLo, highp float roughness)
{
	highp float r = roughness + 1.0;
	highp float k = (r * r) / 8.0; // Epic suggests using this roughness remapping for analytic lights.
	return gaSchlickG1(cosLi, k) * gaSchlickG1(cosLo, k);
}

highp vec3 fresnelSchlick(highp vec3 F0, highp float cosTheta)
{
	return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

mediump vec3 calcLightContribution(highp vec3 lightDir, highp vec3 eyeDir, mediump vec3 diffuseIntensity, mediump vec3 specularIntensity, highp vec3 normal,
	mediump float roughness, mediump float cosEye, mediump float metallic, mediump vec3 diffuse, mediump float matSpecular)
{
	// http://ruh.li/GraphicsCookTorrance.html
	// https://renderman.pixar.com/view/cook-torrance-surface-shader-code
	// https://www.gamedev.net/forums/topic/444098-hlsl-cook-torrance-lighting/
	// https://github.com/Nadrin/PBR/blob/master/data/shaders/hlsl/pbr.hlsl

	const highp float noDiv0 = 1e-5;
	// Constant normal incidence Fresnel factor for all non-metal.
	highp vec3 Fdielectric = vec3(0.2,0.2,0.2);

	highp vec3 halfVector = normalize(lightDir + eyeDir);

	highp float cosLight = max(0.0, dot(normal, lightDir));
	highp float cosHalf = max(0.0, dot(normal, halfVector));

	highp vec3 F0 = mix(Fdielectric, max(vec3(0.1, 0.1, 0.1), diffuse), metallic);
	highp vec3 F = fresnelSchlick(F0, max(0.0, dot(halfVector, eyeDir)));
	F *= matSpecular;
	highp float D = ndfGGX(cosHalf, roughness);
	highp float G = gaSchlickGGX(cosLight, cosEye, roughness);

	// metal has no diffuse contribution
	highp vec3 kd = mix(vec3(1.0, 1.0, 1.0) - F, vec3(0.0, 0.0, 0.0), metallic);
	highp vec3 diffuseBRDF = kd * diffuse;
	highp vec3 specularBRDF = (F * D * G) / max(noDiv0, 4.0 * cosLight * cosEye);

	highp vec3 lightContrib = (diffuseBRDF*diffuseIntensity + specularBRDF*specularIntensity) * cosLight;
	return lightContrib;
}

mediump vec3 cookTorranceOmniDirectional(mediump vec3 diffuse, highp vec3 normalizedNormal, mediump float metallic, 
	mediump float roughness, mediump float matSpecular, mediump vec3 diffuseAmbient, 
	mediump vec3 specularAmbient, highp vec3 eyeDir)
{
	// make a fake light for ambient that points directly at the normal.
	highp vec3 ambLightDir = normalizedNormal;

	highp float cosEye = max(0.0, dot(normalizedNormal, eyeDir));

	return calcLightContribution(ambLightDir, eyeDir, diffuseAmbient, specularAmbient, normalizedNormal, roughness, cosEye, metallic, diffuse, matSpecular);
}

mediump vec3 cookTorrance(mediump vec3 diffuse, highp vec3 normalizedNormal, mediump float metallic, 
	mediump float roughness, mediump float matSpecular, in GHLight lights[kMaxLights], 
	mediump vec3 diffuseAmbient, mediump vec3 specularAmbient, highp vec3 eyeDir)
{
	// lightDir should already be normalized in GHLightMaterialCallback.
	highp float cosEye = max(0.0, dot(normalizedNormal, eyeDir));

	mediump vec3 finalValue = vec3(0.0,0.0,0.0);

#if !defined(SHADER_QUALITY) || SHADER_QUALITY > 0
	for (int i = 0; i < kMaxLights; ++i)
	{
		highp vec3 lightDir = lights[i].mVec;
		mediump vec3 lightIntensity = lights[i].mIntensity;

		finalValue += calcLightContribution(lightDir, eyeDir, lightIntensity, lightIntensity, normalizedNormal, roughness, cosEye, metallic, diffuse, matSpecular);
	}
#endif // SHADER_QUALITY

	// make a fake light for ambient that points directly at the normal.
	highp vec3 ambLightDir = normalizedNormal;
	mediump float ambMetal = max(metallic - 0.5, 0.0); // cheat to get more of the diffuse in there.
	finalValue += calcLightContribution(ambLightDir, eyeDir, diffuseAmbient, specularAmbient, normalizedNormal, roughness, cosEye, ambMetal, diffuse, matSpecular);

	return finalValue;
}

mediump vec3 applyLights(mediump vec3 diffuse, highp vec3 normal, mediump float metallic, 
	mediump float roughness, mediump float matSpecular, in GHLight lights[kMaxLights], 
	mediump vec3 diffuseAmbient, mediump vec3 specularAmbient, highp vec3 eyeDir, mediump float lightIntensityMod)
{
#if !defined(SHADER_QUALITY) || SHADER_QUALITY > 0
	diffuseAmbient *= lightIntensityMod;
	specularAmbient *= lightIntensityMod;
	for (int i = 0; i < kMaxLights; ++i)
	{
		lights[i].mIntensity *= lightIntensityMod;
	}
	return cookTorrance(diffuse, normal, metallic, roughness, matSpecular, lights, diffuseAmbient, specularAmbient, eyeDir);
#else
	// do something cheaper.
	mediump vec3 whiteColor = vec3(lightIntensityMod, lightIntensityMod, lightIntensityMod);
	mediump float lightMult = 2.5;
	diffuseAmbient = min(whiteColor, diffuseAmbient * lightMult);
	specularAmbient = min(whiteColor, specularAmbient * lightMult);
	// omnidirectional doesn't work well with full metal, so bring in some diffuse.
	metallic = min(metallic, 0.5);
	return cookTorranceOmniDirectional(diffuse, normal, metallic, roughness, matSpecular, diffuseAmbient, specularAmbient, eyeDir);
#endif
}
