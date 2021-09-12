// Copyright Golden Hammer Software
#include "GHHeightFieldXMLLoader.h"
#include "GHHeightField.h"
#include "GHUtils/GHResourceFactory.h"
#include "GHTexture.h"
#include "GHXMLNode.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHUtils/GHProfiler.h"

GHHeightFieldXMLLoader::GHHeightFieldXMLLoader(GHResourceFactory& resFactory)
: mResFactory(resFactory)
{
}

void* GHHeightFieldXMLLoader::create(const GHXMLNode& node, GHPropertyContainer& extraData) const
{
    GHPoint2 hfSize(0,0);
    node.readAttrFloatArr("size", hfSize.getArr(), 2);
    
    float nodeDist = 0.1f;
    node.readAttrFloat("nodeDist", nodeDist);
    float maxHeight = 5.0f;
    node.readAttrFloat("maxHeight", maxHeight);
    
    GHHeightField* ret = new GHHeightField((unsigned int)hfSize[0], (unsigned int)hfSize[1], nodeDist);
    
	for (size_t i = 0; i < node.getChildren().size(); ++i)
    {
        loadHFSeg(*node.getChildren()[i], *ret, maxHeight, extraData);
    }
    
    return ret;
}

void GHHeightFieldXMLLoader::loadHFSeg(const GHXMLNode& node, GHHeightField& hf, float maxHeight,
                                       GHPropertyContainer& extraData) const
{
    //    <hfseg texture="choices_hf_x0y0.hff" pos="0 0"/>
    const char* texName = node.getAttribute("texture");
    if (!texName) {
        GHDebugMessage::outputString("No texture specified for height field.");
        return;
    }
    GHTexture* heightTex = (GHTexture*)mResFactory.getCacheResource(texName, &extraData);
    if (!heightTex) {
        GHDebugMessage::outputString("Failed to load texture %s for height field", texName);
        return;
    }
    
    GHPoint2 posStart(0,0);
    node.readAttrFloatArr("pos", posStart.getArr(), 2);
    
    heightTex->acquire();
    unsigned int texWidth,texHeight,texDepth;
    heightTex->getDimensions(texWidth, texHeight, texDepth);
    loadHeightVals(hf, *heightTex, maxHeight, posStart);
    heightTex->release();
}

void GHHeightFieldXMLLoader::loadHeightVals(GHHeightField& ret, GHTexture& tex, float maxHeight,
                                            const GHPoint2& posStart) const
{
    unsigned int texWidth,texHeight,texDepth;
    tex.getDimensions(texWidth, texHeight, texDepth);
    void* texData = 0;
    unsigned int pitch;
    if (!tex.lockSurface(&texData, pitch) || !texData)
    {
        GHDebugMessage::outputString("Failed to lock surface for height field.");
        return;
    }
    
	// if type is float we need to figure out the range of values in order to implement maxHeight.
	float maxFoundHeight = 0.0f;
	if (texDepth == 4 && maxHeight != 0)
	{
		for (unsigned int row = 0; row < texHeight; ++row)
		{
			for (unsigned int col = 0; col < texWidth; ++col)
			{
				void* pixel = tex.getPixel(col, row, texData);
				char* pixelChar = (char*)pixel;

				float nodeHeight = *((float*)(pixelChar));
				if (nodeHeight > maxFoundHeight)
				{
					maxFoundHeight = nodeHeight;
				}
			}
		}
	}

    for (unsigned int row = 0; row < texHeight; ++row) 
    {
        for (unsigned int col = 0; col < texWidth; ++col)
        {
            void* pixel = tex.getPixel(col, row, texData);
            char* pixelChar = (char*) pixel;
            
            float nodeHeight = 0;
			if (texDepth == 4)
			{
				// assume float texture (hff)
				float fval = *((float*)(pixelChar));
				if (maxHeight != 0)
				{
					nodeHeight = (fval / maxFoundHeight)*maxHeight;
				}
				else {
					nodeHeight = fval;
				}
			}
            else if (texDepth == 3)
            {
                // NOTE: this code is far too sensitive to rgb order.
                unsigned char val = *(pixelChar+2);
                float pct = ((float)val)/255.0f;
                nodeHeight = pct*maxHeight;
            }
            else if (texDepth == 2)
            {
                unsigned short val = *((unsigned short*)(pixelChar));
                float pct = ((float)val)/((float)USHRT_MAX);
                nodeHeight = pct*maxHeight;
            }
            else if (texDepth == 1)
            {
                unsigned char val = *(pixelChar);
                float pct = ((float)val)/255.0f;
                nodeHeight = pct*maxHeight;
            }
            ret.setHeight(col + (unsigned int)posStart[0], row + (unsigned int)posStart[1], nodeHeight);
        }
    }
    tex.unlockSurface();
}
