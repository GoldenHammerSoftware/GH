// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHString/GHString.h"
#include <list>

class GHGameCenter;
class GHGameCenterLocalPlayer;
class GHXMLSerializer;
class GHThreadFactory;
class GHMutex;

//In the case where we want to update a score, but we haven't authenticated GameCenter yet,
// we store the score here and try again at a later time
class GHUpdatedScoreCache
{
public:
    GHUpdatedScoreCache(GHGameCenter& gameCenter,
                        const GHGameCenterLocalPlayer& localPlayer,
                        const GHXMLSerializer& serializer,
                        const GHThreadFactory& threadFactory);
    ~GHUpdatedScoreCache(void);
    
    void addScoreUpdate(const char* category, int newScore);
    void addScoreIncrement(const char* category, int increment);
    void flushScores(void);
    
private:
    void saveScores(void);
    
private:
    struct Score
    {
        GHString mCategory;
        int mValue;
        bool mIsIncrement;
        
        Score(void) : mValue(0), mIsIncrement(false) { }
    };
    typedef std::list<Score> ScoreList;
    
private:
    void condenseScores(ScoreList& scoreList);
    void condenseScore(ScoreList& scoreList, ScoreList::iterator& currentIter);
    
private:
    GHGameCenter& mGameCenter;
    const GHGameCenterLocalPlayer& mLocalPlayer;
    const GHXMLSerializer& mSerializer;
    ScoreList mCachedScores;
    GHMutex* mMutex;
};
