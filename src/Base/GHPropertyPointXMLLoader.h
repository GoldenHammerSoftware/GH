// Copyright Golden Hammer Software
#pragma once

#include "GHXMLObjLoader.h"
#include "GHXMLNode.h"
#include "GHMath/GHPoint.h"
#include "GHMath/GHFloat.h"

// <vecNProp val="0 0 0"/>
// templated to support different length points.
template< size_t COUNT >
class GHPropertyPointXMLLoader : public GHXMLObjLoader
{
public:
    GHPropertyPointXMLLoader(void) {}
    
    virtual void* create(const GHXMLNode& node, GHPropertyContainer& extraData) const
    {
		float floatArr[COUNT];
		if (node.readAttrFloatArr("val", floatArr, COUNT))
		{
			GHPoint<float, COUNT>* point = new GHPoint<float, COUNT>();
			memcpy(point->getArr(), floatArr, sizeof(floatArr));
			GHRefCountedType< GHPoint<float, COUNT> >* refCount = new GHRefCountedType< GHPoint<float, COUNT> >(point);
			return new GHProperty(point, refCount);
		}
		return 0;
    }
    GH_NO_POPULATE

	virtual bool save(GHXMLNode& node, void* data, GHPropertyContainer* extraData) const
	{
		if (!data)
		{
			return false;
		}

		//data should be be to a pointer to a GHPoint<float, COUNT>
		GHPoint<float, COUNT>* point = reinterpret_cast<GHPoint<float, COUNT>*>(data);

		int numCharacters = 0;
		for (int i = 0; i < COUNT; ++i)
		{
			int numThisFloat = GHFloat::countCharacters(point->getArr()[i]);
			numCharacters += numThisFloat + 1; //add 1 for the space
		}
		++numCharacters; //null char
		
		char* value = new char[numCharacters];
		
		char* nextValue = value;
		int remainingCharacters = numCharacters;
		for (int i = 0; i < COUNT; ++i)
		{
			int numThisFloat = snprintf(nextValue, remainingCharacters, "%f ", point->getArr()[i]);
			nextValue += numThisFloat;
			remainingCharacters -= numThisFloat;
		}

		node.setAttribute("val", GHString::CHT_REFERENCE, value, GHString::CHT_CLAIM);

		return true;
	}
};
