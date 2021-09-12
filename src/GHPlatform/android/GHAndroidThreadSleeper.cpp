// Copyright 2010 Golden Hammer Software
#include "GHAndroidThreadSleeper.h"
#include <unistd.h>

void GHAndroidThreadSleeper::sleep(int ms)
{
	// usleep takes microseconds, sleep expects milliseconds.
	::usleep(ms*1000);
}
