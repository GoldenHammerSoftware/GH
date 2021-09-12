// Copyright Golden Hammer Software
#pragma once

#include "GHXMLObjLoader.h"

class GHPropertyContainer;
class GHXMLObjFactory;
class GHStringIdFactory;

//<guiNotchedSliderMonitor prop = "GP_DIFFICULTYSLIDERVAL">
//	<notches>
//		<notch val = "0">
//			<transitions>
//				<transitionsGoHere/>
//			</transitions>
//		</notch>
//	</notches>
//</guiNotchedSliderMonitor/>
class GHGUINotchedSliderMonitorXMLLoader : public GHXMLObjLoader
{
public:
	GHGUINotchedSliderMonitorXMLLoader(GHPropertyContainer& appProps, const GHXMLObjFactory& factory, const GHStringIdFactory& hashtable);
	virtual void* create(const GHXMLNode& node, GHPropertyContainer& extraData) const;
	GH_NO_POPULATE

private:
	GHPropertyContainer& mAppProps;
	const GHXMLObjFactory& mXMLObjFactory;
	const GHStringIdFactory& mIdFactory;
};
