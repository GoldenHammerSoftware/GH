
#if (!defined(_WIN32_WINNT) || _WIN32_WINNT >= 0x0602 /*_WIN32_WINNT_WIN8*/)

#include <xaudio2.h>
#include <xaudio2fx.h>
#include <x3daudio.h>

#include <mmreg.h>
#include <mfidl.h>
#include <mfapi.h>
#include <mfmediaengine.h>
#include <mfreadwrite.h>

#else

#include "..\\GHWin32\\GHWin32Include.h"

#include <mmreg.h>
#include <mfidl.h>
#include <mfapi.h>
#include <mfmediaengine.h>
#include <mfreadwrite.h>

#include "..\\..\\..\\contrib\\DirectX\\Include\\xaudio2.h"

#endif
