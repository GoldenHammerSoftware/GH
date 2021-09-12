#include "GHAndroidRatingListener.h"
#include "GHUtils/GHEventMgr.h"
#include "GHGameIdentifiers.h"
#include "GHPlatform/GHDebugMessage.h"

GHAndroidRatingListener::GHAndroidRatingListener(GHJNIMgr& jniMgr, jobject javaObj,
    GHEventMgr& eventMgr)
    : mEventMgr(eventMgr)
    , mJNIMgr(jniMgr)
    , mJavaObj(javaObj)
{
    updateJniFunctions();

    mEventMgr.registerListener(GHGameIdentifiers::M_REQUESTRATING, *this);
}

GHAndroidRatingListener::~GHAndroidRatingListener(void)
{
    mEventMgr.unregisterListener(GHGameIdentifiers::M_REQUESTRATING, *this);
}

void GHAndroidRatingListener::handleMessage(const GHMessage& message)
{
    if (!mRequestMethodId)
    {
        updateJniFunctions();
    }
    if (mRequestMethodId)
    {
        mJNIMgr.getJNIEnv().CallVoidMethod(mJavaObj, mRequestMethodId);
    }
}

void GHAndroidRatingListener::updateJniFunctions(void)
{
    jclass cls = mJNIMgr.getJNIEnv().GetObjectClass(mJavaObj);
    if (cls == 0) {
        GHDebugMessage::outputString("GHAndroidRatingListener can't find java class where methods exist.");
    }

    mRequestMethodId = mJNIMgr.getJNIEnv().GetMethodID(cls, "requestAppRating", "()V");
    if (mRequestMethodId == 0)
    {
        GHDebugMessage::outputString("GHAndroidRatingListener can't find requestAppRating()");
    }
}
