//varying highp vec4 vertexColor;
//
//void main()
//{
//    gl_FragColor = vertexColor; 
//}


//FragmentHeader
//#define varying in
//#define texture2D texture
//#define textureCube texture
//#define gl_FragColor ovr_FragColor
//out vec4 ovr_FragColor;
//FragmentHeader

#define SAMPLE_MODE_COLOR 0
#define SAMPLE_MODE_TEXTURE 1
#define SAMPLE_MODE_TEXTURE_SINGLE_CHANNEL 2
#define SAMPLE_MODE_PARALLAX 3 // unused on Mobile
#define SAMPLE_MODE_RSRM 4

#define MASK_TYPE_NONE 0
#define MASK_TYPE_POSITIONAL 1
#define MASK_TYPE_REFLECTION 2
#define MASK_TYPE_FRESNEL 3
#define MASK_TYPE_PULSE 4 // unused on Mobile

#define BLEND_MODE_ADD 0
#define BLEND_MODE_MULTIPLY 1

varying highp vec3 vertexWorldPos;
varying highp vec3 vertexViewDir;
varying highp vec3 vertexObjPos;
varying highp vec3 vertexNormal;
varying highp vec2 vertexUV;
varying highp vec4 vertexColor;

uniform sampler2D alphaMaskTexture;
uniform sampler2D clothingAlphaMaskTexture;
uniform sampler2D layerSurfaceTextures;
uniform sampler2D layerSurfaceTextures2;
uniform sampler2D layerSurfaceTextures3;

//layout (std140) uniform Parameters
//{
uniform highp mat4 projectorInv;
uniform highp vec4 baseColor;
uniform highp vec4 baseMaskParameters;
uniform highp vec4 baseMaskAxis;
uniform highp vec4 alphaMaskScaleOffset;
uniform highp vec4 clothingAlphaMaskScaleOffset;
uniform highp float Time;
uniform int baseMaskType;
uniform float useProjector;
uniform float useAlpha;
//} params;

//uniform LayerState
//{
uniform int layerCount;
uniform int layerBlendModes[3];
uniform int layerSamplerModes[3];
uniform int layerMaskTypes[3];
uniform highp vec4 layerColors[3];
uniform highp vec4 layerSampleParameters[3];
uniform highp vec4 layerSurfaceScaleOffsets[3];
uniform highp vec4 layerMaskParameters[3];
uniform highp vec4 layerMaskAxes[3];
//} ls;

highp vec3 ComputeColor(
	int sampleMode, highp vec2 uv, highp vec4 color,
	sampler2D surface, highp vec4 surfaceScaleOffset,
	highp vec4 sampleParameters, highp vec3 worldNormal )
{
	if ( sampleMode == SAMPLE_MODE_TEXTURE )
	{
		highp vec2 panning = Time * sampleParameters.xy;
		return texture2D(
			surface,
			( uv + panning ) * surfaceScaleOffset.xy + surfaceScaleOffset.zw
		).rgb * color.rgb;
	}
	if ( sampleMode == SAMPLE_MODE_TEXTURE_SINGLE_CHANNEL )
	{
		highp vec4 channelMask = sampleParameters;
		highp vec4 channels =
			texture2D( surface, uv * surfaceScaleOffset.xy + surfaceScaleOffset.zw );
		return dot( channelMask, channels ) * color.rgb;
	}
	else if ( sampleMode == SAMPLE_MODE_RSRM )
	{
		highp float roughnessMin = sampleParameters.x;
		highp vec3 viewReflect = reflect( -vertexViewDir, worldNormal );
		highp float viewAngle = viewReflect.y * 0.5 + 0.5;
		return texture2D( surface, vec2( roughnessMin, viewAngle ) ).rgb * color.rgb;
	}
	return color.rgb;
}

highp float ComputeMask(
	int maskType, highp vec4 maskParameters,
	highp vec4 maskAxis, highp vec3 worldNormal )
{
	if ( maskType == MASK_TYPE_POSITIONAL )
	{
		highp float centerDistance = maskParameters.x;
		highp float fadeAbove = maskParameters.y;
		highp float fadeBelow = maskParameters.z;
		highp float d = dot( vertexObjPos, maskAxis.xyz );
		if ( d > centerDistance )
		{
			return clamp( 1.0 - ( d - centerDistance ) / fadeAbove, 0.0, 1.0 );
		}
		else
		{
			return clamp( 1.0 - ( centerDistance - d ) / fadeBelow, 0.0, 1.0 );
		}
	}
	else if ( maskType == MASK_TYPE_REFLECTION )
	{
		highp float fadeStart = maskParameters.x;
		highp float fadeEnd = maskParameters.y;
		highp vec3 viewReflect = reflect( -vertexViewDir, worldNormal );
		highp float d = dot( viewReflect, maskAxis.xyz );
		return clamp( 1.0 - ( d - fadeStart ) / ( fadeEnd - fadeStart ), 0.0, 1.0 );
	}
	else if ( maskType == MASK_TYPE_FRESNEL )
	{
		highp float power = maskParameters.x;
		highp float fadeStart = maskParameters.y;
		highp float fadeEnd = maskParameters.z;
		highp float d = 1.0 - max( 0.0, dot( vertexViewDir, worldNormal ) );
		highp float p = pow( d, power );
		return clamp( mix( fadeStart, fadeEnd, p ), 0.0, 1.0 );
	}
	return 1.0;
}

highp vec3 ComputeBlend( int blendMode, highp vec3 dst, highp vec3 src, highp float mask )
{
	if ( blendMode == BLEND_MODE_MULTIPLY )
	{
		return dst * ( src * mask );
	}
	else
	{
		return dst + src * mask;
	}
}

void main()
{
	highp vec2 uv = vertexUV;
	if ( useProjector == 1.0f )
	{
		highp vec4 projectorPos = projectorInv * vec4( vertexWorldPos, 1.0 );
		if ( abs( projectorPos.x ) > 1.0 ||
			 abs( projectorPos.y ) > 1.0 ||
			 abs( projectorPos.z ) > 1.0 )
		{
			discard;
			return;
		}
		uv = projectorPos.xy * 0.5 + 0.5;
	}
	
	highp vec4 color = baseColor;
	highp vec3 worldNormal = normalize( vertexNormal );
	for ( int i = 0; i < layerCount; ++i )
	{
		highp vec3 layerColor;
		if ( i == 0 )
		{
			layerColor = ComputeColor(
				layerSamplerModes[i], uv, layerColors[i], layerSurfaceTextures,
				layerSurfaceScaleOffsets[i], layerSampleParameters[i], worldNormal );
		}
		else if ( i == 1 )
		{
			layerColor = ComputeColor(
				layerSamplerModes[i], uv, layerColors[i], layerSurfaceTextures2,
				layerSurfaceScaleOffsets[i], layerSampleParameters[i], worldNormal );
		}
		else
		{
			layerColor = ComputeColor(
				layerSamplerModes[i], uv, layerColors[i], layerSurfaceTextures3,
				layerSurfaceScaleOffsets[i], layerSampleParameters[i], worldNormal );
		}
		highp float layerMask = ComputeMask(
			layerMaskTypes[i], layerMaskParameters[i], layerMaskAxes[i], worldNormal );
		color.rgb = ComputeBlend( layerBlendModes[i], color.rgb, layerColor, layerMask );
	}
	
	if ( useAlpha == 1.0f )
	{
		color.a *= texture2D(
			alphaMaskTexture,
			uv * alphaMaskScaleOffset.xy + alphaMaskScaleOffset.zw
		).r;
	}
	else if ( useAlpha == 2.0f )
	{
		highp float scaledValue = vertexColor.a * 2.0;
		highp float noAlphaWeight = max( 0.0, 1.0 - scaledValue );
		highp float clothingAlphaWeight = max( 0.0, scaledValue - 1.0 );
		highp float bodyAlphaWeight = 1.0 - noAlphaWeight - clothingAlphaWeight;
		color.a *= texture2D(
			alphaMaskTexture,
			uv * alphaMaskScaleOffset.xy + alphaMaskScaleOffset.zw
		).r * bodyAlphaWeight
       + texture2D(
			clothingAlphaMaskTexture,
			uv
			* clothingAlphaMaskScaleOffset.xy
			+ clothingAlphaMaskScaleOffset.zw
		).r * clothingAlphaWeight
		+ noAlphaWeight;
	}
	
	color.a *= ComputeMask(
		baseMaskType, baseMaskParameters,
		baseMaskAxis, worldNormal );
	gl_FragColor = mix( color, color * vec4( 0.6, 0.6, 0.6, 1.f ), vertexColor.r );

}
