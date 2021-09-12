#pragma once

#include <vector>
#include "GHPlatform/GHTimeVal.h"
#include "GHString/GHString.h"
#include "GHXMLNode.h"

//A templated class for recording keyframes of anything (simple structs).
// Requirements for template arguments:
//	* bool Frame::isIdenticalKeyframe(const Frame&) const
//			(keyframe recorder will not record consecutive frames that are the same)
//			This is expected *not* to compare time.
//			This is a named function rather than operator== in case you otherwise need an operator== that does compare time.
//	* float Frame::mTime
//			This value will be recorded by GHKeyframeRecorder and should not be filled by FrameCollector
//  * bool FrameCollector::operator()(Frame& frame) 
//			a functor that does the specific frame collection Return false if frame can't be collected
//  * GHXMLNode* FrameXMLCollector::operator()(void);
//			a functor that returns a new XML node
//			optional (may pass GHNullFrameXMLCollector)
//			creates a new GHXMLNode. May return null.
// You may store the vector being filled elsewhere
// 
// Todo: add frequency filtering to avoid collecting every frame. 

template<typename KeyFrame>
GHXMLNode* GHNullFrameXMLCollector(const KeyFrame& frame)
{
	return nullptr;
}


template<typename KeyFrame, typename KeyFrameCollector, typename KeyFrameXMLCollector>
class GHKeyframeRecorder 
{
public:
	typedef std::vector<KeyFrame> Keyframes;

	GHKeyframeRecorder(const GHTimeVal& timeVal, 
					   KeyFrameCollector frameCollector, 
					   KeyFrameXMLCollector xmlCollector = GHNullFrameXMLCollector<KeyFrame>())
		: mTimeVal(timeVal)
		, mFrameCollector(frameCollector)
		, mFrameXMLCollector(xmlCollector)
	{
	}

	void startRecording(void)
	{
		mStartTime = mTimeVal.getTime();
	}

	void clearKeyframeList(Keyframes* keyframesToFill = nullptr)
	{
		Keyframes& keyframes = keyframesToFill ? *keyframesToFill : mOwnedKeyframeList;
		keyframes.resize(0);
	}

	void recordKeyframe(Keyframes* keyframesToFill = nullptr)
	{
		Keyframes& keyframes = keyframesToFill ? *keyframesToFill : mOwnedKeyframeList;

		KeyFrame thisFrame;
		thisFrame.mTime = mTimeVal.getTime() - mStartTime;
		if (!mFrameCollector(thisFrame))
		{
			return;
		}

		if (keyframes.size() < 1)
		{
			keyframes.push_back(thisFrame);
		}
		else if (keyframes.back().mTime != thisFrame.mTime)
		{
			if (!thisFrame.isIdenticalKeyframe(keyframes.back()))
			{
				//Since we're skipping any frames that are identical
				// insert an extra keyframe for previous frame to avoid erroneous interpolation
				float sequenceEndTime = mTimeVal.getLastTime() - mStartTime;
				if (keyframes.back().mTime < sequenceEndTime)
				{
					keyframes.push_back(keyframes.back());
					keyframes.back().mTime = sequenceEndTime;
				}

				keyframes.push_back(thisFrame);
			}
		}
	}

	GHXMLNode* collectXML(const char* topNodeName, GHString::CharHandlingType cht, const Keyframes* keyframesToFill = nullptr) const
	{
		const Keyframes& keyframes = keyframesToFill ? *keyframesToFill : mOwnedKeyframeList;

		std::vector<GHXMLNode*> childNodes;
		for (const auto& keyframe : keyframes)
		{
			GHXMLNode* node = mFrameXMLCollector(keyframe);
			if (node)
			{
				char buf[16];
				snprintf(buf, 16, "%f", keyframe.mTime);
				node->setAttribute("time", GHString::CHT_REFERENCE, buf, GHString::CHT_COPY);
				childNodes.push_back(node);
			}
		}

		if (!childNodes.size())
		{
			return nullptr;
		}

		GHXMLNode* topNode = new GHXMLNode;
		if (!topNode)
		{
			for (GHXMLNode* node : childNodes) {
				delete node;
			}
			return nullptr;
		}

		topNode->setName(topNodeName, cht);
		topNode->getChildren().swap(childNodes);

		return topNode;
	}

private:
	Keyframes mOwnedKeyframeList;
	KeyFrameCollector mFrameCollector;
	KeyFrameXMLCollector mFrameXMLCollector;
	const GHTimeVal& mTimeVal;
	float mStartTime{ 0 };
};