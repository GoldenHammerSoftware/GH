#ifndef GH_DEFINES_FILE_INCLUDED
#define GH_DEFINES_FILE_INCLUDED

// The glsl optimizer that we're using automatically inserts #ifdef GL_EXT_shader_texture_lod (specifically this extension)
//	around every function call that needs it. It also operates as if GL_EXT_shader_texture_lod is undefined, meaning it ignores
//  any code that we explicitly put inside such an ifdef. We would like our code to work correctly regardless of whether or not 
//	we run it through the glsl optimizer, so we set a define here that sets our ifdefs accordingly. 
#ifdef GH_USING_GLSL_OPTIMIZER //always true if we are running the optimizer, otherwise false.
	#define GH_SHADER_TEXTURE_LOD_EXTENSION_AVAILABLE // Always defined -- the optimizer will wrap calls so they are safe.
#else
	#ifdef GL_EXT_shader_texture_lod
		#define GH_SHADER_TEXTURE_LOD_EXTENSION_AVAILABLE	// Only define if the extension is available.
	#endif
#endif

//Do basicaly the same thing as the optimizer does
lowp vec4 ghTextureCubeLOD(lowp samplerCube sampler, highp vec3 coord, mediump float lod)
{
#ifdef GH_SHADER_TEXTURE_LOD_EXTENSION_AVAILABLE
	return textureCubeLodEXT(sampler, coord, lod);
#else
	return textureCube(sampler, coord, lod);
#endif
}
#endif // header guard
