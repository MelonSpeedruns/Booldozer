#pragma once

#include "io/KeyframeIO.hpp"
#include "BGRenderDOMNode.hpp"
#include "scene/Camera.hpp"

class LCameraAnimationDOMNode : public LBGRenderDOMNode 
{
private:

    bool mPlaying;
    uint32_t mCurrentFrame;
    
    uint32_t mStartFrame;
    uint32_t mFrameCount;

    uint32_t mNextPosKeyX;
    uint32_t mNextPosKeyY;
    uint32_t mNextPosKeyZ;

    uint32_t mNextTargetKeyX;
    uint32_t mNextTargetKeyY;
    uint32_t mNextTargetKeyZ;

    uint32_t mNextFovKey;

    LTrackCommon mPosFramesX;
    LTrackCommon mPosFramesY;
    LTrackCommon mPosFramesZ;

    LTrackCommon mTargetFramesX;
    LTrackCommon mTargetFramesY;
    LTrackCommon mTargetFramesZ;

    LTrackCommon mUnkownDataFrames;

    LTrackCommon mFovFrames;

    LTrackCommon mZNearFrames;
    LTrackCommon mZFarFrames;

    float mUnknownFloat;

    bool mCameraPosCollapsed;

public:

	typedef LBGRenderDOMNode Super;

	LCameraAnimationDOMNode(std::string name);

    void Load(bStream::CStream* stream);

    void RenderDetailsUI(float dt, LSceneCamera* camera);

/*=== Type operations ===*/
	// Returns whether this node is of the given type, or derives from a node of that type.
	virtual bool IsNodeType(EDOMNodeType type) const override
	{
		if (type == EDOMNodeType::CameraAnim)
			return true;

		return Super::IsNodeType(type);
	}
};