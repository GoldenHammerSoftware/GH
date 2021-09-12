// Copyright Golden Hammer Software
#include "GHRenderPassMembershipXMLLoader.h"
#include "GHRenderPassMembership.h"
#include "GHString/GHHashListParser.h"
#include "GHXMLNode.h"

GHRenderPassMembershipXMLLoader::GHRenderPassMembershipXMLLoader(GHStringIdFactory& hashTable)
: mIdFactory(hashTable)
{
}

void* GHRenderPassMembershipXMLLoader::create(const GHXMLNode& node, GHPropertyContainer& extraData) const
{
    GHRenderPassMembership* ret = new GHRenderPassMembership;
    populate(ret, node, extraData);
    return ret;
}

void GHRenderPassMembershipXMLLoader::populate(void* obj, const GHXMLNode& node, GHPropertyContainer& extraData) const
{
    GHRenderPassMembership* ret = (GHRenderPassMembership*)obj;
    
    bool emptySupportsAll = false;
    if (node.readAttrBool("emptySupportsAll",emptySupportsAll))
    {
        ret->setEmptySupportsAll(emptySupportsAll);
    }
    
    std::vector<GHIdentifier> passList;
    GHHashListParser::parseHashList(node.getAttribute("passes"), mIdFactory, passList);
    for (size_t i = 0; i < passList.size(); ++i)
    {
        ret->addSupportedPass(passList[i]);
    }

	passList.clear();
	GHHashListParser::parseHashList(node.getAttribute("excludedPasses"), mIdFactory, passList);
	for (size_t i = 0; i < passList.size(); ++i)
	{
		ret->addExcludedPass(passList[i]);
	}
}
