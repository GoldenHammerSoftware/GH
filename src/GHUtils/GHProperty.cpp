// Copyright Golden Hammer Software
#include "GHProperty.h"

const GHProperty GHProperty::sInvalidProperty(0);

template<>
void GHProperty::setVal(const int& val)
{
    setVal((float)val);
}

template<>
void GHProperty::setVal(const unsigned int& val)
{
    setVal((float)val);
}

template<>
void GHProperty::setVal(const short& val)
{
    setVal((float)val);
}

template<>
void GHProperty::setVal(const double& val)
{
    setVal((float)val);
}

template<>
void GHProperty::setVal(const bool& val)
{
    float fval = 0.0f;
    if (val) fval = 1.0f;
    setVal<float>(fval);
}
