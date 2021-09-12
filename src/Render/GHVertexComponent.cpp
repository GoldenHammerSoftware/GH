// Copyright Golden Hammer Software
#include "GHVertexComponent.h"
#include "assert.h"

const GHVertexComponent GHVertexComponent::AP_POS = GHVertexComponent(GHVertexComponentShaderID::SI_POS, GHVertexComponentType::CT_FLOAT, 3);
const GHVertexComponent GHVertexComponent::AP_POS2D = GHVertexComponent(GHVertexComponentShaderID::SI_POS, GHVertexComponentType::CT_FLOAT, 2);
const GHVertexComponent GHVertexComponent::AP_NORMAL = GHVertexComponent(GHVertexComponentShaderID::SI_NORMAL, GHVertexComponentType::CT_FLOAT, 3);
const GHVertexComponent GHVertexComponent::AP_BYTENORMAL = GHVertexComponent(GHVertexComponentShaderID::SI_NORMAL, GHVertexComponentType::CT_BYTE, 3);
const GHVertexComponent GHVertexComponent::AP_SHORTNORMAL = GHVertexComponent(GHVertexComponentShaderID::SI_NORMAL, GHVertexComponentType::CT_SHORT, 3);
const GHVertexComponent GHVertexComponent::AP_ENCODEDNORMAL = GHVertexComponent(GHVertexComponentShaderID::SI_NORMAL, GHVertexComponentType::CT_UBYTE, 2);
const GHVertexComponent GHVertexComponent::AP_DIFFUSE = GHVertexComponent(GHVertexComponentShaderID::SI_DIFFUSE, GHVertexComponentType::CT_UBYTE, 4);
const GHVertexComponent GHVertexComponent::AP_SPECULAR = GHVertexComponent(GHVertexComponentShaderID::SI_SPECULAR, GHVertexComponentType::CT_UBYTE, 4);
const GHVertexComponent GHVertexComponent::AP_UV1 = GHVertexComponent(GHVertexComponentShaderID::SI_UV1, GHVertexComponentType::CT_FLOAT, 2);
const GHVertexComponent GHVertexComponent::AP_UV2 = GHVertexComponent(GHVertexComponentShaderID::SI_UV2, GHVertexComponentType::CT_FLOAT, 2);

const char* GHVertexComponent::getStringForShaderID(GHVertexComponentShaderID::Enum val)
{
    // todo: automate this
    if (val == GHVertexComponentShaderID::SI_POS) return "SI_POS";
    if (val == GHVertexComponentShaderID::SI_NORMAL) return "SI_NORMAL";
	if (val == GHVertexComponentShaderID::SI_TANGENT) return "SI_TANGENT";
    if (val == GHVertexComponentShaderID::SI_DIFFUSE) return "SI_DIFFUSE";
    if (val == GHVertexComponentShaderID::SI_SPECULAR) return "SI_SPECULAR";
    if (val == GHVertexComponentShaderID::SI_UV1) return "SI_UV1";
    if (val == GHVertexComponentShaderID::SI_UV2) return "SI_UV2";
	if (val == GHVertexComponentShaderID::SI_BONEWEIGHT) return "SI_BONEWEIGHT";
	if (val == GHVertexComponentShaderID::SI_BONEIDX) return "SI_BONEIDX";

    assert(false);
    return 0;
}

const char* GHVertexComponent::getStringForComponentType(GHVertexComponentType::Enum val)
{
    // todo: automate this too
    if (val == GHVertexComponentType::CT_BYTE) return "CT_BYTE";
    if (val == GHVertexComponentType::CT_UBYTE) return "CT_UBYTE";
    if (val == GHVertexComponentType::CT_SHORT) return "CT_SHORT";
    if (val == GHVertexComponentType::CT_FLOAT) return "CT_FLOAT";
    
    assert(false);
    return 0;
}


