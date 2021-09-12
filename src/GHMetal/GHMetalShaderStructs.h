// This is included by both shaders and cpu code.
// It is a place to share structure definitions.

// Using include guards just in case msl doesn't like #pragma once.
#ifndef GHMetalShaderStructs_h
#define GHMetalShaderStructs_h

// The buffer slots given to each buffer type.
enum GHMTL_BufferIndex
{
    // *** Constant buffer types.
    // vb input
    GHMTL_BI_VPERFRAME = 1, // 0 might go to stage_in?
    GHMTL_BI_VPERTRANS,
    GHMTL_BI_VPERGEO,
    GHMTL_BI_VPERENT,
    // fragment input
    GHMTL_BI_PPERFRAME,
    GHMTL_BI_PPERTRANS,
    GHMTL_BI_PPERGEO,
    GHMTL_BI_PPERENT,
    // *** VB data
    // Leave empty space after stream 0 for other streams.
    GHMTL_BI_VERTEXSTREAM0,
    GHMTL_BI_MAX
};

// The attribute slots attributes to each component type.
enum GHMTL_AttributeIndex
{
    GHMTL_AI_POS = 0,
    GHMTL_AI_NORMAL,
    GHMTL_AI_TANGENT,
    GHMTL_AI_DIFFUSE,
    GHMTL_AI_SPECULAR,
    GHMTL_AI_BONEIDX,
    GHMTL_AI_UV0,
    GHMTL_AI_UV1,
    GHMTL_AI_MAX
};

#endif
