#ifndef _REFLECTIONINCLUDE_
#define _REFLECTIONINCLUDE_

float3 calc4SampleDither(float2 uvCenter, Texture2D tex, SamplerState samp, float bigOffset, float smallOffset)
{
	float2 o = uvCenter;// fmod(floor(uvCenter.xy), 2.0);
	float3 shadTot = 0.0;
	shadTot += tex.Sample(samp, float2(-bigOffset, bigOffset) + o).xyz;
	shadTot += tex.Sample(samp, float2(smallOffset, bigOffset) + o).xyz;
	shadTot += tex.Sample(samp, float2(-bigOffset, -smallOffset) + o).xyz;
	shadTot += tex.Sample(samp, float2(smallOffset, -smallOffset) + o).xyz;
	shadTot *= 0.25;
	return shadTot;
}

float4 calc9SampleDither(float2 uvCenter, Texture2D ShadowTexture, SamplerState ShadowTextureSampler)
{
	float4 shadTot = 0.0;
	float2 shadLookup;
	// offset a little more than a pixel to let the hardware do some blurring.
	// assuming a 1024 input image size.
	float offset = 1.15 / 2048.0;
	for (float x = -1.0; x < 2.0; x += 1.0)
	{
		for (float y = -1.0; y < 2.0; y += 1.0)
		{
			shadLookup = uvCenter;
			shadLookup.x += x * offset;
			shadLookup.y += y * offset;
			float4 shadTest = ShadowTexture.Sample(ShadowTextureSampler, shadLookup);
			shadTest.xyz *= shadTest.w;
			shadTot += shadTest;
		}
	}
	shadTot /= 9.0f;
	return shadTot;
}

// This is a workaround for not having texture2DProj in hlsl.
float2 getProjectedUV(in float4 pos)
{
	const float noDivZeroEpsilon = 0.00001;
	float2 uv;
	uv.x = (((pos.x / (pos.w + noDivZeroEpsilon))) + 0.5);
	uv.y = 1.0 - (((pos.y / (pos.w + noDivZeroEpsilon))) + 0.5);
	uv.x = clamp(uv.x, 0.0, 1.0);
	uv.y = clamp(uv.y, 0.0, 1.0);

#ifdef GH_GLSL
	// projection textures are upside down in glsl.
	uv.y = 1.0 - uv.y;
#endif

	return uv;
}

void applyPixelReflection(in float4 reflectionPos, in Texture2D reflectionTexture, 
						  in SamplerState reflectionSampler, in float reflectionAmt,
						  inout float4 color)
{
	if (reflectionAmt < 0.01) return;	
	float2 reflectionUV = getProjectedUV(reflectionPos);
	//float4 ret = targetTexture.Sample(reflectionTexture, uv);
	float4 reflectionColor = calc9SampleDither(reflectionUV, reflectionTexture, reflectionSampler);

	color.xyz = lerp(color.xyz, reflectionColor.xyz, reflectionAmt);
}

// see if a given worldpos is in shadow
// returns % in shadow.
float getShadowAmount(in float4 shadowPos, in Texture2D shadowTexture, in SamplerState shadowSampler,
	in float3 worldPos, in float3 shadowCenter, in float shadowRadius)
{
	float2 dist2D = abs(shadowCenter.xz - worldPos.xz);
	float dist = length(dist2D);
	dist = saturate(dist-shadowRadius*0.85);
	float shadowDistPct = 1.0-dist;

#ifndef GH_GLSL
	float myDepth = (shadowPos.z*2.0) / shadowPos.w;
#else
	// z/w is in -1 to 1 space, but glDepthRange is more like 0 to 1.
	// so we have to do a conversion.
	float myDepth = ((shadowPos.z / shadowPos.w) + 1.0) / 2.0;
	// Something seems really screwy here.
	// When using shadowpixel.glsl the above line works.
	// When we use a glsl depth buffer instead then it seems like the depth is reversed?
	// There is a high chance that we have more work to do here to figure this out outside of how we are using this for bowling.
	myDepth = 1.0 - myDepth;
#endif

	float2 shadowUV = getProjectedUV(shadowPos);

	/* // block for debugging simpler lookups.
	float shadowColor = shadowTexture.Sample(shadowSampler, shadowUV).r;
	//return shadowColor;
	if (shadowColor < myDepth)
	{
		return 1.0;
	}
	return 0.0;
	//*/

	// offset a little more than a pixel to let the hardware do some blurring.
	// these numbers are total hacks to make bowling look good.
	float shadTot = 0.0;
	float offset = 0.75 / 1024.0;
	float2 shadLookup;
	for (float x = -1.0; x < 2.0; x += 1.0)
	{
		for (float y = -1.0; y < 2.0; y += 1.0)
		{
			shadLookup = shadowUV;
			shadLookup.x += x * offset;
			shadLookup.y += y * offset;
			float shadTest = shadowTexture.Sample(shadowSampler, shadLookup).r;
			if (shadTest < myDepth)
			{
				shadTot += 1.0;
			}
		}
	}
	shadTot /= 9.0;
	return shadTot * shadowDistPct;
}

#endif
