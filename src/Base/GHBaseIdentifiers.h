// Copyright Golden Hammer Software
#if defined(GHPUTSTRINGSINTABLES) || defined(GHPUTDECLARATIONS) || !defined(_GHBASEIDENTIFIERS_H_)
#ifndef _GHBASEIDENTIFIERS_H_
#define _GHBASEIDENTIFIERS_H_
#endif

#include "GHString/GHEnum.h"

GHIDENTIFIERBEGIN(GHBaseIdentifiers)
GHIDENTIFIER(STATEMACHINE) // a state machine prop val

GHIDENTIFIER(RUMBLESUPPORTED) //true if the platform supports rumble, false otherwise
GHIDENTIFIER(RUMBLEDISABLED) //true if the player has elected to disable rumble

GHIDENTIFIER(M_WEBNEWSREAD) // notification that the player tried to read the news
GHIDENTIFIER(M_WEBNEWSVERSION) // notification that we have the version id of web news. stored in INTVAL
GHIDENTIFIER(M_WEBNEWSCONTENTS) //notification of contents of new web news. stored in VAL
GHIDENTIFIER(HASWEBNEWS) // 1 if there is unread web news

GHIDENTIFIER(M_AUDIODEVICERESET) //notification that the audio device has been reset
GHIDENTIFIER(M_NEWAUDIODEVICEAVAILABLE) //notification that there is a new audio device available
GHIDENTIFIER(MP_DESIREDAUDIODEVICEGUIDSTR) //a wide string containing the device guid

GHIDENTIFIER(P_TRANSFORM) //a transform property

GHIDENTIFIER(M_RELEASEFILETOKEN) // make any code forget the filetoken stored in VAL

GHIDENTIFIER(FULLSCREENSUPPORTED) // true if the platform supports separate windowed/fullscreen modes
GHIDENTIFIER(ISFULLSCREEN) // true if the app is not currently fullscreen

GHIDENTIFIER(M_TOGGLEFULLSCREEN) // force a switch between fullscreen and windowed if WINDOWEDSUPPORTED.
GHIDENTIFIER(M_FULLSCREENENABLED) // notificiation that we left fullscreen
GHIDENTIFIER(M_FULLSCREENDISABLED) // notification that we entered fullscreen

GHIDENTIFIER(P_NOLOADASPROPERTY) // a sometimes supported flag to make a property loader not load as a property.

GHIDENTIFIEREND

#include "GHString/GHEnumEnd.h"

#endif
