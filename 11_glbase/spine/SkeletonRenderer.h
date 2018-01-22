/******************************************************************************
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
#include <functional>
#include <tuple>
#include <spine/spine.h>
#include <LcEuclid.h>

namespace spine {

class AttachmentVertices;

#ifndef SPINEMESHARGS
#define SPINEMESHARGS
//typedef std::tuple
//<
//	  void*				// 0: texture
//	, float*			// 1: pos
//	, float*			// 2: color
//	, float*			// 3: texcoord
//	, int				// 4: stride
//	, int				// 5: vertex count
//	, unsigned short*	// 6: idx_buf
//	, int				// 7: idx count
//> SpineMeshArgs;
//
//enum
//{
//	SPINEMESHARGS_TEXTURE = 0,
//	SPINEMESHARGS_POSITION,
//	SPINEMESHARGS_DIFFUSE ,
//	SPINEMESHARGS_TEXCOORD,
//	SPINEMESHARGS_STRIDE  ,
//	SPINEMESHARGS_VTX_NUM ,
//	SPINEMESHARGS_IDX_BUF ,
//	SPINEMESHARGS_IDX_NUM ,
//};

struct SpineMeshArgs
{
	void*			texture;
	float*			pos;
	float*			dif;
	float*			tex;
	int				stride;
	int				vtx_num;
	unsigned short*	idx_buf;
	int				idx_num;
};

#endif

typedef std::function<void(const SpineMeshArgs&)> SpineRender;

/* Draws a skeleton. */
class SkeletonRenderer {
public:

	static SkeletonRenderer* createWithData (spSkeletonData* skeletonData, bool ownsSkeletonData = false);
	static SkeletonRenderer* createWithFile (const std::string& skeletonDataFile, spAtlas* atlas, float scale = 1);
	static SkeletonRenderer* createWithFile (const std::string& skeletonDataFile, const std::string& atlasFile, float scale = 1);

	virtual void update (float deltaTime);
	virtual void draw (const SpineRender& render);
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
	spAtlas* m_atlas{};
	spSkeleton* m_skeleton{};
	spAttachmentLoader* _attachmentLoader;
	bool _premultipliedAlpha;
	float _timeScale;
	bool _debugSlots;
	bool _debugBones;

	int      m_vtx_num{};
	LCXVEC2* m_vtx_pos{};		// position
	COLORF4* m_vtx_dif{};		// position

	COLORF4  m_color {COLORF4::WHITE};
	LCXVEC3  m_pos   {LCXVEC3::ZERO};
	LCXVEC3  m_scl   {LCXVEC3::ONE};
public:
	void color(const COLORF4& c) { m_color = c; }
	const COLORF4& color() { return m_color; }
	const LCXVEC3& getPosition() const { return m_pos; }
	const LCXVEC3& getScale() const { return m_scl; }
};

}

#endif /* SPINE_SKELETONRENDERER_H_ */
