// Copyright 2010 Golden Hammer Software
#pragma once

class GHRagdollGenerator;
class GHRagdollToolNode;
class GHPropertyContainer;
class GHEntity;
class GHStringHashTable;

class GHRagdollToolCommandHandler
{
public:
    GHRagdollToolCommandHandler(GHRagdollGenerator& generator,
                                GHRagdollToolNode*& currentSelection,
                                GHStringHashTable& hashtable);
    
    bool setShape(const char* shape);
    bool setHeight(const char* height);
    bool setWidth(const char* width);
    bool setDepth(const char* depth);
    bool setRadius(const char* radius);
    bool setLimit(const char* limit);
    bool setConstraintType(const char* constraintType);
    bool setAxis(const char* axis);
    bool playAnim(const char* anim);
    
private:
    GHRagdollGenerator& mGenerator;
    GHRagdollToolNode*& mCurrentSelection;
    GHStringHashTable& mHashtable;
};
