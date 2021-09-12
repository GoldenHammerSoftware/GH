// Copyright Golden Hammer Software
#pragma once

#include "GHVertexComponentShaderID.h"
#include "GHVertexComponentType.h"

// little struct to define one component of a vert
class GHVertexComponent
{
public:
    GHVertexComponent(GHVertexComponentShaderID::Enum sid, GHVertexComponentType::Enum type, int count)
    : mID(sid), mType(type), mCount(count) {}

    // given a shader comp id, get thesave string
    static const char* getStringForShaderID(GHVertexComponentShaderID::Enum val);
    static const char* getStringForComponentType(GHVertexComponentType::Enum val);
    
public:
    // shader handle
    GHVertexComponentShaderID::Enum mID;
    // type of data
    GHVertexComponentType::Enum mType;
    // number of instances of mType in a row
    int mCount;

public:
    // handles to often used ComponentDef values.
    static const GHVertexComponent AP_POS;
    static const GHVertexComponent AP_POS2D;
    static const GHVertexComponent AP_NORMAL;
    static const GHVertexComponent AP_BYTENORMAL;
    static const GHVertexComponent AP_SHORTNORMAL;
    static const GHVertexComponent AP_ENCODEDNORMAL;
    static const GHVertexComponent AP_DIFFUSE;
    static const GHVertexComponent AP_SPECULAR;
    static const GHVertexComponent AP_UV1;
    static const GHVertexComponent AP_UV2;
};
