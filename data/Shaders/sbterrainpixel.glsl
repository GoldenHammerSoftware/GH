varying vec2 v_uv1;
varying vec2 v_grounduv;
varying vec3 v_cliffuv;

uniform sampler2D Texture0;
uniform sampler2D CliffTexture;
uniform sampler2D GroundTexture;
uniform sampler2D NormTexture;

uniform sampler2D ShadowTexture;
varying vec4 v_ShadowPos;

uniform sampler2D WideShadowTexture;
varying vec4 v_WideShadowPos;

uniform sampler2D TrailTexture;
varying vec4 v_TrailPos;

uniform vec3 LightDir0;
varying vec3 v_modelEyeDir;

float createShadowBlend(float pixelDist, float shadDist)
{
	float shadDiff = pixelDist - shadDist;
    float shadClamp = clamp(shadDiff, 0.0, 1.0);
    return shadClamp;
    //return smoothstep(0.0, 0.15, shadClamp);
    /*
	float isNoShadow = step(shadDiff, -0.01);
	float shadClamp = smoothstep(-0.01, 0.015, shadDiff);
	float inShadowVal = isNoShadow + shadClamp;
	return clamp(inShadowVal, 0.0, 1.0);
    */
}

float shadLookup(vec2 offSet, vec4 shadowPos, sampler2D tex, float shadDist)
{
	vec4 offsetShadProj = shadowPos + vec4(offSet.xy, 0.0, 0.0);
	vec2 offsetUV = offsetShadProj.xy / offsetShadProj.w;
	float pixelVal = texture2D(tex, offsetUV).x;
	
	float blendVal = createShadowBlend(pixelVal, shadDist);
	return blendVal;
}

float calc4SampleDitherShadow(vec2 shadCenter, vec4 shadowPos,
                              sampler2D tex, float bigOffset, float smallOffset)
{
	float shadDist = shadowPos.z / shadowPos.w;
    vec2 o = (shadCenter - vec2(2.0,2.0)) * floor(shadCenter*vec2(0.5,0.5));
    
    float shadTot = 0.0;
	shadTot += shadLookup(vec2(-bigOffset, bigOffset) + o, shadowPos, tex, shadDist);
	shadTot += shadLookup(vec2(smallOffset, bigOffset) + o, shadowPos, tex, shadDist);
	shadTot += shadLookup(vec2(-bigOffset, -smallOffset) + o, shadowPos, tex, shadDist);
	shadTot += shadLookup(vec2(smallOffset, -smallOffset) + o, shadowPos, tex, shadDist);
	shadTot *= 0.25;
	return shadTot;
}

float calcSingleSampleShadow(vec2 shadCenter, vec4 shadowPos, sampler2D tex)
{
	vec2 offsetUV = shadCenter;
	float shadDist = shadowPos.z / shadowPos.w;

	float pixelVal = texture2D(tex, offsetUV).x;
	float blendVal = createShadowBlend(pixelVal, shadDist);
	return blendVal;
}

float calcSpecular(vec4 blendColor, vec3 norm, vec3 modelEyeDir, vec3 lightDir, float specularPower)
{
	// a bigger specularPower means a smaller highlight
	vec3 eyeReflect = reflect(modelEyeDir, norm);
	float specular = pow(max(0.0, dot(eyeReflect, lightDir)), specularPower);
	// we want less specular on rocks.
	specular *= (blendColor.x + (blendColor.y * 0.2));
	specular *= 0.15;
	return specular;
}

float calcWideShadow(vec4 wideshadowPos, float shadInBorder, sampler2D wideShadowTexture)
{
	vec2 wideShadowUV = wideshadowPos.xy / wideshadowPos.w;
	float wideshadTot = calc4SampleDitherShadow(wideShadowUV, wideshadowPos, wideShadowTexture, 0.4*0.002, 0.2*0.002);
	float wideshadBlend = wideshadTot * 0.5 + 0.5;
    
	// apply wide shadow if we are not in the wide border and are in the short border.
	float shadInWideBorder = step(0.95, wideShadowUV.x) + step(0.95, wideShadowUV.y) +
    step(wideShadowUV.x, 0.05) + step(wideShadowUV.y, 0.05);
	return clamp(wideshadBlend + shadInWideBorder + step(shadInBorder, 0.9), 0.0, 1.0);
}

float calcOneSampleWideShadow(vec4 wideshadowPos, float shadInBorder, sampler2D wideShadowTexture)
{
	vec2 wideShadowUV = wideshadowPos.xy / wideshadowPos.w;
	float wideshadTot = texture2D(wideShadowTexture, wideShadowUV).x;
	float wideshadBlend = wideshadTot * 0.5 + 0.5;
    
	// apply wide shadow if we are not in the wide border and are in the short border.
	float shadInWideBorder = step(0.95, wideShadowUV.x) + step(0.95, wideShadowUV.y) +
    step(wideShadowUV.x, 0.05) + step(wideShadowUV.y, 0.05);
	return clamp(wideshadBlend + shadInWideBorder + step(shadInBorder, 0.9), 0.0, 1.0);
}

vec4 calcCliffColor(vec3 normal, sampler2D cliffTexture, vec3 cliffUV)
{
	vec3 blendNorm;
	blendNorm.x = abs(normal.x);
	blendNorm.y = abs(normal.y);
	blendNorm.z = abs(normal.z);
    
	float normTot = blendNorm.x + blendNorm.y + blendNorm.z;
	float normMult = 1.0 / normTot;
	blendNorm *= normMult;
    
	vec4 cliffxColor = texture2D(cliffTexture, cliffUV.yz);
    vec4 cliffyColor = texture2D(cliffTexture, cliffUV.xz);
    vec4 cliffzColor = texture2D(cliffTexture, cliffUV.xy);
    vec4 cliffColor = cliffxColor *blendNorm.x + cliffyColor*blendNorm.y + cliffzColor*blendNorm.z;
    
    vec4 cliffFarColor = texture2D(cliffTexture, vec2(cliffUV.x * 0.1, cliffUV.z * 0.1));
    cliffColor = cliffColor*0.5 + cliffFarColor*0.5;
    
	return cliffColor;
}

void main()
{
	vec3 norm = texture2D(NormTexture, v_uv1).xyz;
	norm = norm * 2.0 - 1.0;
    
	vec4 cliffColor = calcCliffColor(norm, CliffTexture, v_cliffuv);
    
	vec4 blendColor = texture2D(Texture0, v_uv1);
    // todo: triplanar texturing.
	vec4 groundColor = texture2D(GroundTexture, v_grounduv);
	vec4 groundFarColor = texture2D(GroundTexture, v_grounduv*0.01);
	groundColor = groundColor*0.75 + groundFarColor*0.25;
    
	// cliff vs snow
	vec4 color = mix(cliffColor, groundColor, blendColor.x);
	// lightmap
	color.xyz *= blendColor.z;
    
    // high res shadow
	vec2 shadCenter = v_ShadowPos.xy / v_ShadowPos.w;
	float shadTot = calc4SampleDitherShadow(shadCenter, v_ShadowPos, ShadowTexture, 0.6*0.002, 0.3*0.002);
    //float shadTot = calcSingleSampleShadow(shadCenter, v_ShadowPos, ShadowTexture);
	float shadBlend = shadTot;//shadTot * 0.5 + 0.5;
    
	// fade out the shadow on the edge
	// if we're on the border, come up with a value bigger than 1.
	float shadInBorder = step(0.95, shadCenter.x) + step(0.95, shadCenter.y) + step(shadCenter.x, 0.05) + step(shadCenter.y, 0.05);
	// multiply color by the shadow value, unless we are on the border.
	color.xyz *= clamp(shadBlend + shadInBorder, 0.0, 1.0);
    
	color.xyz *= calcWideShadow(v_WideShadowPos, shadInBorder, WideShadowTexture);
    
	vec2 trailUV = v_TrailPos.xy / v_TrailPos.w;
	vec4 trailColor = texture2D(TrailTexture, trailUV);
    
	vec3 trailMinus = vec3(0.2, 0.2, 0.05);
    trailMinus *= trailColor.x * blendColor.x;
    color.xyz -= trailMinus;
    
	vec3 specDir = vec3(0.0, -1.0, 0.0);
	color.xyz += calcSpecular(blendColor, norm, v_modelEyeDir, specDir, 80.0);
	color.xyz += calcSpecular(blendColor, norm, v_modelEyeDir, specDir, 2.0);
    
	gl_FragColor = color;
}
