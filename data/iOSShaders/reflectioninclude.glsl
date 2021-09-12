
mediump vec4 calc4SampleDither(mediump vec4 reflectionPos, sampler2D reflectionTexture)
{
	mediump vec2 offset = vec2(0.0,0.0);
	/*
	highp vec2 offset = fract(reflectionPos.xy*0.5);
	offset.y += offset.x;
	if (offset.y > 1.1)
	{
		offset.y = 0.0;
	}
	*/

	mediump vec4 shadTot = vec4(0.0, 0.0, 0.0, 0.0);
	mediump vec4 shadLookup;
	mediump float offsetMod = (1.0/1024.0)*reflectionPos.w;

	shadLookup = reflectionPos;
	shadLookup.xy += (offset + vec2(-1.5, 0.5))*offsetMod;
	shadTot += texture2DProj(reflectionTexture, shadLookup);

	shadLookup = reflectionPos;
	shadLookup.xy += (offset + vec2(0.5, 0.5))*offsetMod;
	shadTot += texture2DProj(reflectionTexture, shadLookup);

	shadLookup = reflectionPos;
	shadLookup.xy += (offset + vec2(-1.5, -1.5))*offsetMod;
	shadTot += texture2DProj(reflectionTexture, shadLookup);

	shadLookup = reflectionPos;
	shadLookup.xy += (offset + vec2(0.5, -1.5))*offsetMod;
	shadTot += texture2DProj(reflectionTexture, shadLookup);

	shadTot *= 0.25;
	return shadTot;
}

// https://developer.nvidia.com/gpugems/GPUGems/gpugems_ch11.html
mediump vec4 calc9SampleDither(mediump vec4 reflectionPos, sampler2D reflectionTexture)
{
	mediump vec4 shadTot = vec4(0.0, 0.0, 0.0, 0.0);
	mediump vec4 shadLookup;
	mediump float offset = 1.5 * (1.0/1024.0) * reflectionPos.w;
	for (highp float x = -1.0; x < 2.0; x += 1.0)
	{
		for (highp float y = -1.0; y < 2.0; y += 1.0)
		{
			shadLookup = reflectionPos;
			shadLookup.x += x * offset;
			shadLookup.y += y * offset;
			highp vec4 shadTest = texture2DProj(reflectionTexture, shadLookup);
			shadTot += shadTest;
		}
	}
	shadTot /= 9.0;
	return shadTot;
}

mediump vec4 applyPixelReflection(mediump vec4 reflectionPos, sampler2D reflectionTexture, mediump float reflectionAmt, mediump vec4 color)
{
	//lowp vec3 reflectionColor = texture2DProj(reflectionTexture, reflectionPos).rgb;
	//lowp vec3 reflectionColor = calc9SampleDither(reflectionPos, reflectionTexture).rgb;
	mediump vec3 reflectionColor = calc4SampleDither(reflectionPos, reflectionTexture).rgb;

	mediump vec4 outColor;
    outColor.w = color.w;
	outColor.xyz = mix(color.xyz, reflectionColor.xyz, reflectionAmt);

	return outColor;
}

mediump vec4 calcVertexReflection(mediump mat4 viewProj, mediump vec4 worldPos)
{
	mediump vec4 reflectionPos = viewProj * worldPos;
	reflectionPos.xy /= 2.0;
    reflectionPos.xy += reflectionPos.w/2.0;
	return reflectionPos;
}

#ifdef SHADER_SUPPORTS_SHADOW
bool testInShadow(mediump float myDepth, mediump float testDepth)
{
	return testDepth < myDepth;
}

mediump float getShadowAmount(highp vec4 shadowPos, sampler2D shadowTexture, 
	highp vec4 worldPos, mediump vec3 shadowCenter, mediump float shadowRadius)
{
	mediump vec2 dist2D = abs(shadowCenter.xz - worldPos.xz);
	mediump float dist = length(dist2D);
	dist = clamp(dist-shadowRadius*0.85, 0.0, 1.0);
	mediump float shadowDistPct = 1.0-dist;

	// z/w is in -1 to 1 space, but glDepthRange is more like 0 to 1.
	// so we have to do a conversion.
	highp float myDepth = ((shadowPos.z / shadowPos.w) + 1.0)/2.0;
	// Something seems really screwy here.
	// When using shadowpixel.glsl the above line works.
	// When we use a glsl depth buffer instead then it seems like the depth is reversed?
	// There is a high chance that we have more work to do here to figure this out outside of how we are using this for bowling.
	myDepth = 1.0 - myDepth;

	/* // debug code to limit lookups
	highp float dummy = texture2DProj(shadowTexture, shadowPos).r;
	if (testInShadow(myDepth, dummy)) return 1.0;
	return 0.0;
	*/

	// offset a little more than a pixel to let the hardware do some blurring.
	// these numbers are total hacks to make bowling look good.
	mediump float shadTot = 0.0;
	mediump float offset = 0.75 / 2048.0;
	mediump vec4 shadLookup;
	for (mediump float x = -1.0; x < 2.0; x += 1.0)
	{
		for (mediump float y = -1.0; y < 2.0; y += 1.0)
		{
			shadLookup = shadowPos;
			shadLookup.x += x * offset;
			shadLookup.y += y * offset;
			highp float shadTest = texture2DProj(shadowTexture, shadLookup).r;
			if (testInShadow(myDepth, shadTest))
			{
				shadTot += 1.0;
			}
		}
	}
	shadTot /= 9.0;
	return shadTot * shadowDistPct;
}
#endif
