﻿/******************************************************************************
 * Spine Runtimes Software License v2.5
 *
 * Copyright (c) 2013-2016, Esoteric Software
 * All rights reserved.
 *
 * You are granted a perpetual, non-exclusive, non-sublicensable, and
 * non-transferable license to use, install, execute, and perform the Spine
 * Runtimes software and derivative works solely for personal or internal
 * use. Without the written permission of Esoteric Software (see Section 2 of
 * the Spine Software License Agreement), you may not (a) modify, translate,
 * adapt, or develop new applications using the Spine Runtimes or otherwise
 * create derivative works or improvements of the Spine Runtimes or (b) remove,
 * delete, alter, or obscure any trademarks or any copyright, trademark, patent,
 * or other intellectual property or proprietary rights notices on or in the
 * Software, including any copy thereof. Redistributions in binary or source
 * form must include this license and terms.
 *
 * THIS SOFTWARE IS PROVIDED BY ESOTERIC SOFTWARE "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL ESOTERIC SOFTWARE BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES, BUSINESS INTERRUPTION, OR LOSS OF
 * USE, DATA, OR PROFITS) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
 * IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *****************************************************************************/

#ifndef SPINE_SKELETONRENDERER_H_
#define SPINE_SKELETONRENDERER_H_

#include "ogl_util.h"
#include <spine/spine.h>

namespace spine {

class AttachmentVertices;

/* Draws a skeleton. */
class SkeletonRenderer {
public:

	static SkeletonRenderer* createWithData (spSkeletonData* skeletonData, bool ownsSkeletonData = false);
	static SkeletonRenderer* createWithFile (const std::string& skeletonDataFile, spAtlas* atlas, float scale = 1);
	static SkeletonRenderer* createWithFile (const std::string& skeletonDataFile, const std::string& atlasFile, float scale = 1);

	virtual void update (float deltaTime);
	virtual void draw (void);
	virtual void drawDebug (void);
	virtual LCXRECT getBoundingBox () const;

	spSkeleton* getSkeleton();

	void setTimeScale(float scale);
	float getTimeScale() const;

	/*  */
	void setDebugSlotsEnabled(bool enabled);
	bool getDebugSlotsEnabled() const;

	void setDebugBonesEnabled(bool enabled);
	bool getDebugBonesEnabled() const;

	// --- Convenience methods for common Skeleton_* functions.
	void updateWorldTransform ();

	void setToSetupPose ();
	void setBonesToSetupPose ();
	void setSlotsToSetupPose ();

	/* Returns 0 if the bone was not found. */
	spBone* findBone (const std::string& boneName) const;
	/* Returns 0 if the slot was not found. */
	spSlot* findSlot (const std::string& slotName) const;

	/* Sets the skin used to look up attachments not found in the SkeletonData defaultSkin. Attachments from the new skin are
	 * attached if the corresponding attachment from the old skin was attached. Returns false if the skin was not found.
	 * @param skin May be empty string ("") for no skin.*/
	bool setSkin (const std::string& skinName);
	/** @param skin May be 0 for no skin.*/
	bool setSkin (const char* skinName);

	/* Returns 0 if the slot or attachment was not found. */
	spAttachment* getAttachment (const std::string& slotName, const std::string& attachmentName) const;
	/* Returns false if the slot or attachment was not found.
	 * @param attachmentName May be empty string ("") for no attachment. */
	bool setAttachment (const std::string& slotName, const std::string& attachmentName);
	/* @param attachmentName May be 0 for no attachment. */
	bool setAttachment (const std::string& slotName, const char* attachmentName);

    // --- BlendProtocol
    virtual void setBlendFunc (const BlendFunc& blendFunc);
    virtual const BlendFunc& getBlendFunc () const;
    virtual void setOpacityModifyRGB (bool value);
    virtual bool isOpacityModifyRGB () const;

public:
	SkeletonRenderer ();
	SkeletonRenderer (spSkeletonData* skeletonData, bool ownsSkeletonData = false);
	SkeletonRenderer (const std::string& skeletonDataFile, spAtlas* atlas, float scale = 1);
	SkeletonRenderer (const std::string& skeletonDataFile, const std::string& atlasFile, float scale = 1);

	virtual ~SkeletonRenderer ();

	void initWithData (spSkeletonData* skeletonData, bool ownsSkeletonData = false);
	void initWithJsonFile (const std::string& skeletonDataFile, spAtlas* atlas, float scale = 1);
	void initWithJsonFile (const std::string& skeletonDataFile, const std::string& atlasFile, float scale = 1);
    void initWithBinaryFile (const std::string& skeletonDataFile, spAtlas* atlas, float scale = 1);
    void initWithBinaryFile (const std::string& skeletonDataFile, const std::string& atlasFile, float scale = 1);

	virtual void initialize ();

protected:
	void setSkeletonData (spSkeletonData* skeletonData, bool ownsSkeletonData);
	virtual AttachmentVertices* getAttachmentVertices (spRegionAttachment* attachment) const;
	virtual AttachmentVertices* getAttachmentVertices (spMeshAttachment* attachment) const;

	bool _ownsSkeletonData;
	spAtlas* _atlas;
	spAttachmentLoader* _attachmentLoader;
	BlendFunc _blendFunc {0x0302, 0x0303};

#if defined(WIN32)
  #pragma warning(push)
  #pragma warning(disable : 4251)
#endif
	std::vector<float> _worldVertices;
#if defined(WIN32)
  #pragma warning(pop)
#endif

	bool _premultipliedAlpha;
	spSkeleton* _skeleton;
	float _timeScale;
	bool _debugSlots;
	bool _debugBones;

	COLORF4	m_color {COLORF4::WHITE};
	bool		m_use_two_color {false};
	LCXVEC3		m_pos{0,0,0};
	LCXVEC3		m_scl{1,1,1};
public:
	const LCXVEC3 getPosition() const { return m_pos; }
	const LCXVEC3 getScale() const { return m_scl; }
};

}

#endif /* SPINE_SKELETONRENDERER_H_ */