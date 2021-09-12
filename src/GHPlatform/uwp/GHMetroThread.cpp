// Copyright 2010 Golden Hammer Software
#include "GHMetroThread.h"
using namespace Windows::System::Threading;

void GHMetroThread::runThread(GHRunnable& runnable)
{
	// assigning runnable to a member var may or may not be necessary.
	// couldn't figure out any other way to get the variable to the RunAsync lambda.
	mRunnable = &runnable;
	
	ThreadPool::RunAsync(ref new WorkItemHandler([this](Windows::Foundation::IAsyncAction^ operation)
	{  
		mRunnable->run();
	}));
}

