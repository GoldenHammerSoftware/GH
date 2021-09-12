// Copyright Golden Hammer Software
#if defined(GHPUTSTRINGSINTABLES) || !defined(_GHMATERIALCALLBACKTYPE_H_)
#ifndef _GHMATERIALCALLBACKTYPE_H_
    #define _GHMATERIALCALLBACKTYPE_H_
#endif

#include "GHString/GHEnum.h"

// if the order changes, we will need to change the hlsl shaders.
GHENUMBEGIN(GHMaterialCallbackType)
GHENUMVAL(CT_PERFRAME, 0)
GHENUM(CT_PERTRANS)
GHENUM(CT_PERGEO)
GHENUM(CT_PERENT)
GHENUM(CT_MAX)
GHENUMEND

#include "GHString/GHEnumEnd.h"

#endif
