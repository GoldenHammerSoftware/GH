#!/bin/bash
git submodule update --init --remote
git lfs update init
git pull --recurse-submodules
#submodules are in a detached head state.  that's how they are supposed to work.
#for our purposes we want them to be in origin master instead.

