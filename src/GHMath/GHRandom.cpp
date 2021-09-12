// Copyright Golden Hammer Software
#include "GHRandom.h"
#include <stdlib.h>

float GHRandom::getNormalizedRandVal(void)
{
    return ((float)::rand())/(float)RAND_MAX;
}

int GHRandom::getRandInt(int max)
{
    return ::rand()%max;
}

float GHRandom::getRandFloat(float max)
{
    return getNormalizedRandVal()*max;
}

