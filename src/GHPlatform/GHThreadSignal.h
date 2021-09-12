#pragma once

class GHMutex;

// Equivalent of pthread_cond
// Used to signal a waiting thread to continue.
class GHThreadSignal
{
public:
	virtual ~GHThreadSignal(void) = default;

	virtual bool wait(GHMutex& mutex) = 0;
	virtual bool signal(void) = 0;
};

