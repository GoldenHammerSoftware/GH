// Copyright Golden Hammer Software
#pragma once

class GHTransition;
class GHMessageHandler;

// dumping ground for useful functions for dealing with threads
class GHThreadUtil
{
public:
    // create a transition that launches another transition via messages
    static GHTransition* createMessageWrapperTransition(GHTransition& targetTrans, GHMessageHandler& handler);
};
