#pragma once

#include "GHUtils/GHProperty.h"
#include "GHUtils/GHPropertyContainer.h"
#include "GHString/GHIdentifier.h"


//Simple utility for a common pattern we've been using to avoid reallocating certain properties > sizeof(void*) 
// which are owned by the property map (ie, which need ref counted ptrs stored in the property). 
// Similar to std::map::operator[]: 
//  Creates the object with ref counted ptr if it doesn't exist, otherwise uses existing property. 
//  Returns a reference to the underlying value. 
//Requires there to be a default ctor on the object type.
template<typename T>
T& GHGetSharedRefCountedProperty(GHPropertyContainer& props, const GHIdentifier& propId)
{
	const GHProperty& ret = props.getProperty(propId);
	if (ret.isValid())
	{
		return *(T*)ret;
	}

	T* t = new T;
	props.setProperty(propId, GHProperty(t, new GHRefCountedType<T>(t)));
	return *t;
}

//Extra convenience wrapper for GHIdentifier
inline GHIdentifier& GHGetSharedIdentifierProperty(GHPropertyContainer& props, const GHIdentifier& propId)
{
	return GHGetSharedRefCountedProperty<GHIdentifier>(props, propId);
}

