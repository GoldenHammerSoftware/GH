uniform mat4 ModelViewProj;

uniform highp mat4 WorldToModel;

uniform highp vec3 viewPos;

uniform highp mat4 meshPose[64];

attribute highp vec4 position;
attribute highp vec3 normal;
attribute highp vec2 uv1;
attribute highp vec4 boneIndices;
attribute highp vec4 boneWeights;
attribute highp vec4 VertexColor;

varying highp vec3 vertexWorldPos;
varying highp vec3 vertexViewDir;
varying highp vec3 vertexObjPos;
varying highp vec3 vertexNormal;
varying highp vec2 vertexUV;
varying highp vec4 vertexColor;

void main()
{
	highp vec4 vertexPose = vec4(0,0,0,0);
	vertexPose = ( meshPose[ int( boneIndices.x ) ] * position ) * boneWeights.x;
	vertexPose += ( meshPose[ int( boneIndices.y ) ] * position ) * boneWeights.y;
	vertexPose += ( meshPose[ int( boneIndices.z ) ] * position ) * boneWeights.z;
	vertexPose += ( meshPose[ int( boneIndices.w ) ] * position ) * boneWeights.w;

	highp vec4 normal4 = vec4( normal, 0.0 );
	highp vec4 normalPose = vec4(0,0,0,0);
	normalPose = meshPose[ int( boneIndices.x ) ] * normal4 * boneWeights.x;
	normalPose += meshPose[ int( boneIndices.y ) ] * normal4 * boneWeights.y;
	normalPose += meshPose[ int( boneIndices.z ) ] * normal4 * boneWeights.z;
	normalPose += meshPose[ int( boneIndices.w ) ] * normal4 * boneWeights.w;
	normalPose = normalize( normalPose );

    gl_Position = ModelViewProj * vertexPose;

	vertexWorldPos = ( WorldToModel * vertexPose ).xyz; //change: was ModelMatrix
	vertexViewDir = normalize( viewPos - vertexWorldPos ); //change
	vertexObjPos = position.xyz;
	vertexNormal = ( WorldToModel * normalPose ).xyz; //change was ModelMatrix
	vertexUV = uv1;
	vertexColor = VertexColor;
}

