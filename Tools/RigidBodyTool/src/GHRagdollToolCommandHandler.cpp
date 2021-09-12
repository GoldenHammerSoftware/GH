// Copyright 2010 Golden Hammer Software
#include "GHRagdollToolCommandHandler.h"
#include "GHRagdollGenerator.h"
#include "GHRagdollToolNode.h"
#include "GHEntity.h"
#include "GHEntityHashes.h"
#include "GHModel.h"
#include "GHStringHashTable.h"

GHRagdollToolCommandHandler::GHRagdollToolCommandHandler(GHRagdollGenerator& generator,
                                                         GHRagdollToolNode*& currentSelection,
                                                         GHStringHashTable& hashtable)
: mGenerator(generator)
, mCurrentSelection(currentSelection)
, mHashtable(hashtable)
{
    
}

bool GHRagdollToolCommandHandler::setShape(const char* shape)
{
    if (!mCurrentSelection) {
        return false;
    }
    
    const int bufsize = 256;
    char buf[bufsize];
    int i = 0;
    for (; *shape && i <= bufsize-1; ++i, ++shape)
    {
        buf[i] = toupper(*shape);
    }
    buf[i] = 0;
    
    return mCurrentSelection->setShape(buf);
}

bool GHRagdollToolCommandHandler::setHeight(const char* height)
{
    if (!mCurrentSelection) { return false; }
    
    return mCurrentSelection->setExtent(1, atof(height));
}

bool GHRagdollToolCommandHandler::setWidth(const char* width)
{
    if (!mCurrentSelection) { return false; }
    return mCurrentSelection->setExtent(0, atof(width));
}

bool GHRagdollToolCommandHandler::setDepth(const char* depth)
{
    if (!mCurrentSelection) { return false; }
    return mCurrentSelection->setExtent(2, atof(depth));
}

bool GHRagdollToolCommandHandler::setRadius(const char* radius)
{
    if (!mCurrentSelection) { return false; }
    return mCurrentSelection->setRadius(atof(radius));
}

bool GHRagdollToolCommandHandler::setLimit(const char* limit)
{
    if (!mCurrentSelection) { return false; }
    return mCurrentSelection->setLimit(limit);
}

bool GHRagdollToolCommandHandler::setConstraintType(const char* constraintType)
{
    if (!mCurrentSelection) { return false; }
    return mCurrentSelection->setConstraintType(constraintType);
}

bool GHRagdollToolCommandHandler::setAxis(const char* axis)
{
    if (!mCurrentSelection) { return false; }
    return mCurrentSelection->setAxis(axis);
}

bool GHRagdollToolCommandHandler::playAnim(const char* anim)
{
    if (!mGenerator.getTargetEnt()) { return false ;}
    GHModel* model = mGenerator.getTargetEnt()->mProperties.getProperty(GHEntityHashes::MODEL);
    if (!model) { return false; }
    
    if (strlen(anim) == 0) {
        model->setAnim(0);
    }
    else {
        model->setAnim(mHashtable.generateHash(anim));
    }
    return true;
}
