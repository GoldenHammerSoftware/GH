// Copyright Golden Hammer Software
#pragma once

#include <vector>
#include "GHStlUtil.h"

// A widget to hang on to things and then delete them.
template< typename T >
class GHOwnershipList
{
public:
	~GHOwnershipList(void) 
    {
        // we delete in reverse order because it's more likely to not hit issues.
        typename std::vector<T*>::reverse_iterator iter;
        for (iter = mObjList.rbegin(); iter != mObjList.rend(); ++iter)
        {
            delete *iter;
        }
    }
    
    void push_back(T* obj) { mObjList.push_back(obj); }

protected:
    std::vector<T*> mObjList;
};
