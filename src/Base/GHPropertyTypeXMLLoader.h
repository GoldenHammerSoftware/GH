// Copyright Golden Hammer Software
#pragma once

#include "GHXMLObjLoader.h"
#include "GHUtils/GHProperty.h"
#include "GHPlatform/GHRefCounted.h"
#include "GHXMLObjFactory.h"
#include "GHXMLNode.h"
#include "GHPlatform/GHDebugMessage.h"

namespace GHPropertyTypeXMLLoaderNamespace
{
	template<typename T, bool hasDefaultConstructor>
	class DefaultCreator;

	template<typename T>
	class DefaultCreator<T, true>
	{
	public:
		void* createDefault(void)
		{
			T* val = new T();
			return new GHProperty(val, new GHRefCountedType<T>(val));
		}
	};

	template<typename T>
	class DefaultCreator<T, false>
	{
	public:
		void* createDefault(void)
		{
			GHDebugMessage::outputString("Failed to load some thing from a GHPropertyTypeXMLLoader. You must have a child XML block for the thing to load from.");
			return 0;
		}
	};
}


// templated loader that can turn anything loaded from xml into a ref counted property.
template <typename T, bool hasDefaultConstructor=true>
class GHPropertyTypeXMLLoader : public GHXMLObjLoader
{
public:
    GHPropertyTypeXMLLoader(const GHXMLObjFactory& factory)
    : mObjFactory(factory)
    { }
    
    virtual void* create(const GHXMLNode& node, GHPropertyContainer& extraData) const
    {
        const GHXMLNode::NodeList& children = node.getChildren();
        T* val = 0;
        if (!children.size()) {
            // if the format has no children, we expect an empty T constructor.
			return GHPropertyTypeXMLLoaderNamespace::DefaultCreator<T, hasDefaultConstructor>().createDefault();
        }
        else {
            val = (T*)mObjFactory.load(*children[0], &extraData);
        }
        return new GHProperty(val, new GHRefCountedType<T>(val));
    }
    
    GH_NO_POPULATE

private:
    const GHXMLObjFactory& mObjFactory;
};
