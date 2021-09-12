// Copyright Golden Hammer Software
#pragma once

// place to put utility functions for controllers in the gui.
namespace GHGUIController
{
    static float calcPriority(int depth) { return 1000000.0f - depth; }
}
