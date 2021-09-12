#include "ghdefines.hlsl"

cbuffer PerEntBuffer : register(register_perent)
{
	float4x4 GUIInWorldTransform;
};

float4 positionGUIVertex(float2 gui2d)
{
	float4 guiPos4 = float4(gui2d.x, gui2d.y, 0.0, 1.0);
	float4 reprojectedPos = mul(GUIInWorldTransform, guiPos4);
	return reprojectedPos;
}
