// Copyright 2010 Golden Hammer Software
#include "GHUpdatedScoreCache.h"
#include "GHGameCenterLocalPlayer.h"
#include "GHGameCenter.h"
#include "GHXMLSerializer.h"
#include "GHXMLNode.h"
#include "GHPlatform/GHMutex.h"
#include "GHPlatform/GHThreadFactory.h"

const static int sCacheHash = 4132;

GHUpdatedScoreCache::GHUpdatedScoreCache(GHGameCenter& gameCenter,
                                         const GHGameCenterLocalPlayer& localPlayer,
                                         const GHXMLSerializer& serializer,
                                         const GHThreadFactory& threadFactory)
: mGameCenter(gameCenter)
, mLocalPlayer(localPlayer)
, mSerializer(serializer)
, mMutex(threadFactory.createMutex())
{
    GHXMLNode* savedScores = mSerializer.loadEncryptedFile("ghcs", sCacheHash);
    if (savedScores)
    {
        const std::vector<GHXMLNode*>& children = savedScores->getChildren();
        size_t numChildren = children.size();
        for (size_t i = 0; i < numChildren; ++i)
        {
            const GHXMLNode* node = children[i];
                    
            mCachedScores.push_back(Score());
            Score& score = mCachedScores.back();
            score.mCategory.setConstChars(node->getName().getChars(), GHString::CHT_COPY);
            node->readAttrInt("val", score.mValue);
            node->readAttrBool("isinc", score.mIsIncrement);
        }
        delete savedScores;
    }
}

GHUpdatedScoreCache::~GHUpdatedScoreCache(void)
{
    delete mMutex;
}

void GHUpdatedScoreCache::addScoreUpdate(const char* category, int newScore)
{
    GHScopedMutex scopedMutex(mMutex);
    mCachedScores.push_back(Score());
    Score& score = mCachedScores.back();
    score.mCategory.setConstChars(category, GHString::CHT_COPY);
    score.mValue = newScore;
    score.mIsIncrement = false;
    saveScores();
}

void GHUpdatedScoreCache::addScoreIncrement(const char* category, int increment)
{
    GHScopedMutex scopedMutex(mMutex);
    mCachedScores.push_back(Score());
    Score& score = mCachedScores.back();
    score.mCategory.setConstChars(category, GHString::CHT_COPY);
    score.mValue = increment;
    score.mIsIncrement = true;
    saveScores();
}

void GHUpdatedScoreCache::flushScores(void)
{
    if (!mCachedScores.size()) {
        return;
    }
    
    if (!mLocalPlayer.isAuthenticated()) {
        return;
    }
    
    {
        GHScopedMutex scopedMutex(mMutex);
        ScoreList scoresToFlush = mCachedScores;
        mCachedScores.resize(0);
        saveScores();
        
        //combine any scores that are to the same category
        condenseScores(scoresToFlush);
        
        ScoreList::iterator iter = scoresToFlush.begin(), iterEnd = scoresToFlush.end();
    
        for (; iter != iterEnd; ++iter)
        {
            const Score& score = *iter;
            if (score.mIsIncrement) {
                mGameCenter.submitScoreIncrement(score.mCategory.getChars(), score.mValue);
            }
            else {
                mGameCenter.submitScore(score.mCategory.getChars(), score.mValue);
            }
        }
    }
}

void GHUpdatedScoreCache::condenseScores(ScoreList& scoreList)
{
    ScoreList::iterator iter = scoreList.begin(), iterEnd = scoreList.end();
    
    for (; iter != iterEnd; ++iter)
    {
        Score& score = *iter;
        
        ScoreList::iterator laterIter = iter;
        ++laterIter;
        while (laterIter != iterEnd)
        {
            Score& laterScore = *laterIter;
            
            //Combine only if in the same category
            if (score.mCategory == laterScore.mCategory)
            {
                if (laterScore.mIsIncrement)
                {
                    //this should work regardless of whether the original score is a value reset or an increment
                    score.mValue += laterScore.mValue;
                }
                else
                {
                    //If a value reset comes after an increment, then it should replace it.
                    score.mIsIncrement = false;
                    score.mValue = laterScore.mValue;
                }
                laterIter = scoreList.erase(laterIter);
            }
            else ++laterIter;
        }
    }
}

void GHUpdatedScoreCache::saveScores(void)
{
    const int BUF_SZ = 8;
    char buf[BUF_SZ];
    
    GHXMLNode savedScores;
    savedScores.setName("ghcs", GHString::CHT_REFERENCE);
    
    std::list<Score>::iterator iter = mCachedScores.begin(), iterEnd = mCachedScores.end();
    for (; iter != iterEnd; ++iter)
    {
        const Score& score = *iter;
        
        GHXMLNode* child = new GHXMLNode;
        child->setName(score.mCategory.getChars(), GHString::CHT_REFERENCE);
        snprintf(buf, BUF_SZ, "%d", score.mValue);
        child->setAttribute("val", GHString::CHT_REFERENCE, buf, GHString::CHT_COPY);
        child->setAttribute("isinc", GHString::CHT_REFERENCE, score.mIsIncrement ? "true" : "false", GHString::CHT_REFERENCE);
        savedScores.addChild(child);
    }
    
    mSerializer.writeEncryptedFile("ghcs", savedScores, sCacheHash);
}
