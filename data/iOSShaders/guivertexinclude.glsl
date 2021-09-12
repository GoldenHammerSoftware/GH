uniform mat4 GUIInWorldTransform;

vec4 positionGUIVertex(vec2 gui2d)
{
	vec4 guiPos4 = vec4(gui2d.x, gui2d.y, 0.0, 1.0);
	vec4 reprojectedPos = GUIInWorldTransform * guiPos4;
	return reprojectedPos;
}
