// Copyright Golden Hammer Software
#pragma once

// When enabling GHMETAL also change the type of MGHView in
//  MainWindow.xib and MainWindow-iPad.xib.
// It doesn't seem to pick up changes to what GHiOSView is.

#ifdef GHMETAL
    #include "GHiOSViewMetal.h"
    typedef GHiOSViewMetal GHiOSView;
#else
    #include "GHiOSViewOgles.h"
    typedef GHiOSViewOgles GHiOSView;
#endif
