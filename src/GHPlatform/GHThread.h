// Copyright Golden Hammer Software
#pragma once

class GHRunnable;

class GHThread
{
public:
	typedef enum
	{
		TP_DEFAULT,
		TP_HIGH,
		TP_LOW
	} Priority;

public:
	virtual ~GHThread(void) {}
	virtual void runThread(GHRunnable& runnable) = 0;
};
