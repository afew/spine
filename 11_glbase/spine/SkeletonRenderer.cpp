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

#include <algorithm>
#if defined(__APPLE__)
  #define GL_GLEXT_PROTOTYPES
  #import <OpenGLES/ES2/glext.h>
#else
  #include <GLES2/gl2.h>
#endif
#include <spine/extension.h>
#include "SkeletonRenderer.h"
#include "AttachmentVertices.h"
#include "Cocos2dAttachmentLoader.h"

using std::min;
using std::max;

namespace spine {

SkeletonRenderer* SkeletonRenderer::createWithData (spSkeletonData* skeletonData, bool ownsSkeletonData) {
	SkeletonRenderer* node = new SkeletonRenderer(skeletonData, ownsSkeletonData);
	return node;
}

SkeletonRenderer* SkeletonRenderer::createWithFile (const std::string& skeletonDataFile, spAtlas* atlas, float scale) {
	SkeletonRenderer* node = new SkeletonRenderer(skeletonDataFile, atlas, scale);
	return node;
}

SkeletonRenderer* SkeletonRenderer::createWithFile (const std::string& skeletonDataFile, const std::string& atlasFile, float scale) {
	SkeletonRenderer* node = new SkeletonRenderer(skeletonDataFile, atlasFile, scale);
	return node;
}

void SkeletonRenderer::initialize () {

	setOpacityModifyRGB(true);

	AttachmentVertices* attachmentVertices{};
	int max_vertex_count = 8;	// 6 verticies * 16 rectangle

	m_vtx_num = 0;
	for (int i = 0, n = m_skeleton->slotsCount; i < n; ++i) {
		spSlot* slot = m_skeleton->drawOrder[i];
		if (!slot->attachment) continue;

		switch (slot->attachment->type) {
		case SP_ATTACHMENT_REGION: {
			spRegionAttachment* attachment = (spRegionAttachment*)slot->attachment;
			attachmentVertices = getAttachmentVertices(attachment);
			break;
		}
		case SP_ATTACHMENT_MESH: {
			spMeshAttachment* attachment = (spMeshAttachment*)slot->attachment;
			attachmentVertices = getAttachmentVertices(attachment);
			break;
		}
		default:
			continue;
		}

		m_vtx_num += attachmentVertices->_vtx_num;
		if(max_vertex_count<attachmentVertices->_vtx_num)
			max_vertex_count = attachmentVertices->_vtx_num;
	}

	m_vtx_pos = new LCXVEC2[m_vtx_num]{};
	m_vtx_dif = new COLORF4[m_vtx_num]{};
}

void SkeletonRenderer::setSkeletonData (spSkeletonData *skeletonData, bool ownsSkeletonData) {
	m_skeleton = spSkeleton_create(skeletonData);
	_ownsSkeletonData = ownsSkeletonData;
}

SkeletonRenderer::SkeletonRenderer ()
	: m_atlas(nullptr), _attachmentLoader(nullptr), _debugSlots(false), _debugBones(false), _timeScale(1) {
}

SkeletonRenderer::SkeletonRenderer (spSkeletonData *skeletonData, bool ownsSkeletonData)
	: m_atlas(nullptr), _attachmentLoader(nullptr), _debugSlots(false), _debugBones(false), _timeScale(1) {
	initWithData(skeletonData, ownsSkeletonData);
}

SkeletonRenderer::SkeletonRenderer (const std::string& skeletonDataFile, spAtlas* atlas, float scale)
	: m_atlas(nullptr), _attachmentLoader(nullptr), _debugSlots(false), _debugBones(false), _timeScale(1) {
	initWithJsonFile(skeletonDataFile, atlas, scale);
}

SkeletonRenderer::SkeletonRenderer (const std::string& skeletonDataFile, const std::string& atlasFile, float scale)
	: m_atlas(nullptr), _attachmentLoader(nullptr), _debugSlots(false), _debugBones(false), _timeScale(1) {
	initWithJsonFile(skeletonDataFile, atlasFile, scale);
}

SkeletonRenderer::~SkeletonRenderer () {
	if (_ownsSkeletonData) spSkeletonData_dispose(m_skeleton->data);
	spSkeleton_dispose(m_skeleton);
	if (m_atlas) spAtlas_dispose(m_atlas);
	if (_attachmentLoader) spAttachmentLoader_dispose(_attachmentLoader);
	if(m_vtx_pos) delete [] m_vtx_pos;
	if(m_vtx_dif) delete [] m_vtx_dif;
}

void SkeletonRenderer::initWithData (spSkeletonData* skeletonData, bool ownsSkeletonData) {
	setSkeletonData(skeletonData, ownsSkeletonData);

	initialize();
}

void SkeletonRenderer::initWithJsonFile (const std::string& skeletonDataFile, spAtlas* atlas, float scale) {
    m_atlas = atlas;
	_attachmentLoader = SUPER(Cocos2dAttachmentLoader_create(m_atlas));

	spSkeletonJson* json = spSkeletonJson_createWithLoader(_attachmentLoader);
	json->scale = scale;
	spSkeletonData* skeletonData = spSkeletonJson_readSkeletonDataFile(json, skeletonDataFile.c_str());
	if(!skeletonData) {
		//, json->error ? json->error : "Error reading skeleton data.");
		return;
	}
	spSkeletonJson_dispose(json);

	setSkeletonData(skeletonData, true);

	initialize();
}

void SkeletonRenderer::initWithJsonFile (const std::string& skeletonDataFile, const std::string& atlasFile, float scale) {
	m_atlas = spAtlas_createFromFile(atlasFile.c_str(), 0);
	if(!m_atlas) {
		//, "Error reading atlas file.");
		return;
	}

	_attachmentLoader = SUPER(Cocos2dAttachmentLoader_create(m_atlas));

	spSkeletonJson* json = spSkeletonJson_createWithLoader(_attachmentLoader);
	json->scale = scale;
	spSkeletonData* skeletonData = spSkeletonJson_readSkeletonDataFile(json, skeletonDataFile.c_str());
	if(!skeletonData) {
		//, json->error ? json->error : "Error reading skeleton data file.");
		return;
	}
	spSkeletonJson_dispose(json);

	setSkeletonData(skeletonData, true);

	initialize();
}

void SkeletonRenderer::initWithBinaryFile (const std::string& skeletonDataFile, spAtlas* atlas, float scale) {
    m_atlas = atlas;
    _attachmentLoader = SUPER(Cocos2dAttachmentLoader_create(m_atlas));

    spSkeletonBinary* binary = spSkeletonBinary_createWithLoader(_attachmentLoader);
    binary->scale = scale;
    spSkeletonData* skeletonData = spSkeletonBinary_readSkeletonDataFile(binary, skeletonDataFile.c_str());
    if(!skeletonData) {
		//, binary->error ? binary->error : "Error reading skeleton data file.");
		return;
	}
    spSkeletonBinary_dispose(binary);

    setSkeletonData(skeletonData, true);

    initialize();
}

void SkeletonRenderer::initWithBinaryFile (const std::string& skeletonDataFile, const std::string& atlasFile, float scale) {
    m_atlas = spAtlas_createFromFile(atlasFile.c_str(), 0);
    if(!m_atlas) {
		//, "Error reading atlas file.");
		return;
	}

    _attachmentLoader = SUPER(Cocos2dAttachmentLoader_create(m_atlas));

    spSkeletonBinary* binary = spSkeletonBinary_createWithLoader(_attachmentLoader);
    binary->scale = scale;
    spSkeletonData* skeletonData = spSkeletonBinary_readSkeletonDataFile(binary, skeletonDataFile.c_str());
    if(!skeletonData) {
		//, binary->error ? binary->error : "Error reading skeleton data file.");
		return;
	}
    spSkeletonBinary_dispose(binary);

    setSkeletonData(skeletonData, true);

    initialize();
}


void SkeletonRenderer::update (float deltaTime) {
	spSkeleton_update(m_skeleton, deltaTime * _timeScale);
}

void SkeletonRenderer::draw (const SpineRender& render) {
	int stored_blend_src = 0;
	int stored_blend_dst = 0;
	int stored_blend_use = 0;
	int stored_depth_test= 0;
	glGetIntegerv(GL_BLEND_SRC_ALPHA, &stored_blend_src);
	glGetIntegerv(GL_BLEND_DST_ALPHA, &stored_blend_dst);
	stored_blend_use = (int)glIsEnabled(GL_BLEND);
	stored_depth_test= (int)glIsEnabled(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);

	m_skeleton->r = m_color.r;
	m_skeleton->g = m_color.g;
	m_skeleton->b = m_color.b;
	m_skeleton->a = m_color.a;

	COLORF4 color;
	AttachmentVertices* attachmentVertices = nullptr;
	for (int i = 0, n = m_skeleton->slotsCount; i < n; ++i) {
		spSlot* slot = m_skeleton->drawOrder[i];
		if (!slot->attachment) continue;

		switch (slot->attachment->type) {
		case SP_ATTACHMENT_REGION: {
			spRegionAttachment* attachment = (spRegionAttachment*)slot->attachment;
			spRegionAttachment_computeWorldVertices(attachment, slot->bone, (float*)&m_vtx_pos[0]);
			attachmentVertices = getAttachmentVertices(attachment);
			color = COLORF4((const float*)&attachment->r);
			break;
		}
		case SP_ATTACHMENT_MESH: {
			spMeshAttachment* attachment = (spMeshAttachment*)slot->attachment;
			spMeshAttachment_computeWorldVertices(attachment, slot, (float*)&m_vtx_pos[0]);
			attachmentVertices = getAttachmentVertices(attachment);
			color = COLORF4((const float*)&attachment->r);
			break;
		}
		default:
			continue;
		}

		color.a *= m_skeleton->a * slot->a;
		float multiplier = _premultipliedAlpha ? color.a : 1.0F;
		color.r *= m_skeleton->r * slot->r * multiplier;
		color.g *= m_skeleton->g * slot->g * multiplier;
		color.b *= m_skeleton->b * slot->b * multiplier;

		for (int v = 0, vn = attachmentVertices->_vtx_num; v < vn; ++v) {
			m_vtx_dif[v] = color;
		}

		int blend_src = 0;
		int blend_dst = 0;
		switch (slot->data->blendMode) {
		case SP_BLEND_MODE_ADDITIVE:
			blend_src = _premultipliedAlpha ? GL_ONE : GL_SRC_ALPHA;
			blend_dst = GL_ONE;
			break;
		case SP_BLEND_MODE_MULTIPLY:
			blend_src = GL_DST_COLOR;
			blend_dst = GL_ONE_MINUS_SRC_ALPHA;
			break;
		case SP_BLEND_MODE_SCREEN:
			blend_src = GL_ONE;
			blend_dst = GL_ONE_MINUS_SRC_COLOR;
			break;
		default:
			blend_src = _premultipliedAlpha ? GL_ONE : GL_SRC_ALPHA;
			blend_dst = GL_ONE_MINUS_SRC_ALPHA;
		}
		glBlendFunc(blend_src, blend_dst);

		void* texture   = attachmentVertices->_texture;
		float* pos      = (float*)&m_vtx_pos[0];
		float* dif      = (float*)&m_vtx_dif[0];
		float* tex      = attachmentVertices->_vtx_tex;
		int    stride   = 0;
		int     vtx_num = attachmentVertices->_vtx_num;
		USHORT* idx_buf = attachmentVertices->_idx_buf;
		int     idx_num = attachmentVertices->_idx_num;
		render(SpineMeshArgs{texture, pos, dif, tex, stride, vtx_num, idx_buf, idx_num});
	}

	glBlendFunc(stored_blend_src, stored_blend_dst);
	if(!stored_blend_use)
		glDisable(GL_BLEND);

	if(stored_depth_test)
		glEnable(GL_DEPTH_TEST);

	if (_debugSlots || _debugBones) {
		drawDebug();
	}
}

void SkeletonRenderer::drawDebug (void) {

    //if (_debugSlots) {
    //    // Slots.
    //    // DrawPrimitives::setDrawColor4B(0, 0, 255, 255);
    //    glLineWidth(1);
    //    Vec2 points[4];
    //    V3F_C4B_T2F_Quad quad;
    //    for (int i = 0, n = m_skeleton->slotsCount; i < n; i++) {
    //        spSlot* slot = m_skeleton->drawOrder[i];
    //        if (!slot->attachment || slot->attachment->type != SP_ATTACHMENT_REGION) continue;
    //        spRegionAttachment* attachment = (spRegionAttachment*)slot->attachment;
    //        spRegionAttachment_computeWorldVertices(attachment, slot->bone, _worldVertices);
    //        points[0] = Vec2(_worldVertices[0], _worldVertices[1]);
    //        points[1] = Vec2(_worldVertices[2], _worldVertices[3]);
    //        points[2] = Vec2(_worldVertices[4], _worldVertices[5]);
    //        points[3] = Vec2(_worldVertices[6], _worldVertices[7]);
    //        drawNode->drawPoly(points, 4, true, COLORF4::BLUE);
    //    }
    //}
    //if (_debugBones) {
    //    // Bone lengths.
    //    glLineWidth(2);
    //    for (int i = 0, n = m_skeleton->bonesCount; i < n; i++) {
    //        spBone *bone = m_skeleton->bones[i];
    //        float x = bone->data->length * bone->a + bone->worldX;
    //        float y = bone->data->length * bone->c + bone->worldY;
    //        drawNode->drawLine(Vec2(bone->worldX, bone->worldY), Vec2(x, y), COLORF4::RED);
    //    }
    //    // Bone origins.
    //    auto color = COLORF4::BLUE; // Root bone is blue.
    //    for (int i = 0, n = m_skeleton->bonesCount; i < n; i++) {
    //        spBone *bone = m_skeleton->bones[i];
    //        drawNode->drawPoint(Vec2(bone->worldX, bone->worldY), 4, color);
    //        if (i == 0) color = COLORF4::GREEN;
    //    }
    //}

}

AttachmentVertices* SkeletonRenderer::getAttachmentVertices (spRegionAttachment* attachment) const {
	return (AttachmentVertices*)attachment->rendererObject;
}

AttachmentVertices* SkeletonRenderer::getAttachmentVertices (spMeshAttachment* attachment) const {
	return (AttachmentVertices*)attachment->rendererObject;
}

LCXRECT SkeletonRenderer::getBoundingBox () const {

	float minX = FLT_MAX, minY = FLT_MAX, maxX = -FLT_MAX, maxY = -FLT_MAX;
	float scaleX = m_scl.x, scaleY = m_scl.y;
	for (int i = 0; i < m_skeleton->slotsCount; ++i) {
		spSlot* slot = m_skeleton->slots[i];
		if (!slot->attachment) continue;
		int verticesCount;
		if (slot->attachment->type == SP_ATTACHMENT_REGION) {
			spRegionAttachment* attachment = (spRegionAttachment*)slot->attachment;
			spRegionAttachment_computeWorldVertices(attachment, slot->bone, (float*)&m_vtx_pos[0]);
			verticesCount = 8/2;
		} else if (slot->attachment->type == SP_ATTACHMENT_MESH) {
			spMeshAttachment* mesh = (spMeshAttachment*)slot->attachment;
			spMeshAttachment_computeWorldVertices(mesh, slot, (float*)&m_vtx_pos[0]);
			verticesCount = mesh->super.worldVerticesLength/2;
		} else
			continue;
		for (int ii = 0; ii < verticesCount; ++ii) {
			float x = m_vtx_pos[ii].x * scaleX, y = m_vtx_pos[ii].y * scaleY;
			minX = min(minX, x);
			minY = min(minY, y);
			maxX = max(maxX, x);
			maxY = max(maxY, y);
		}
	}

	if (minX == FLT_MAX) minX = minY = maxX = maxY = 0;
	return LCXRECT(m_pos.x + minX, m_pos.y + minY, maxX - minX, maxY - minY);
}

// --- Convenience methods for Skeleton_* functions.

void SkeletonRenderer::updateWorldTransform () {
	spSkeleton_updateWorldTransform(m_skeleton);
}

void SkeletonRenderer::setToSetupPose () {
	spSkeleton_setToSetupPose(m_skeleton);
}
void SkeletonRenderer::setBonesToSetupPose () {
	spSkeleton_setBonesToSetupPose(m_skeleton);
}
void SkeletonRenderer::setSlotsToSetupPose () {
	spSkeleton_setSlotsToSetupPose(m_skeleton);
}

spBone* SkeletonRenderer::findBone (const std::string& boneName) const {
	return spSkeleton_findBone(m_skeleton, boneName.c_str());
}

spSlot* SkeletonRenderer::findSlot (const std::string& slotName) const {
	return spSkeleton_findSlot(m_skeleton, slotName.c_str());
}

bool SkeletonRenderer::setSkin (const std::string& skinName) {
	return spSkeleton_setSkinByName(m_skeleton, skinName.empty() ? 0 : skinName.c_str()) ? true : false;
}
bool SkeletonRenderer::setSkin (const char* skinName) {
	return spSkeleton_setSkinByName(m_skeleton, skinName) ? true : false;
}

spAttachment* SkeletonRenderer::getAttachment (const std::string& slotName, const std::string& attachmentName) const {
	return spSkeleton_getAttachmentForSlotName(m_skeleton, slotName.c_str(), attachmentName.c_str());
}
bool SkeletonRenderer::setAttachment (const std::string& slotName, const std::string& attachmentName) {
	return spSkeleton_setAttachment(m_skeleton, slotName.c_str(), attachmentName.empty() ? 0 : attachmentName.c_str()) ? true : false;
}
bool SkeletonRenderer::setAttachment (const std::string& slotName, const char* attachmentName) {
	return spSkeleton_setAttachment(m_skeleton, slotName.c_str(), attachmentName) ? true : false;
}

spSkeleton* SkeletonRenderer::getSkeleton () {
	return m_skeleton;
}

void SkeletonRenderer::setTimeScale (float scale) {
	_timeScale = scale;
}
float SkeletonRenderer::getTimeScale () const {
	return _timeScale;
}

void SkeletonRenderer::setDebugSlotsEnabled (bool enabled) {
	_debugSlots = enabled;
}
bool SkeletonRenderer::getDebugSlotsEnabled () const {
	return _debugSlots;
}

void SkeletonRenderer::setDebugBonesEnabled (bool enabled) {
	_debugBones = enabled;
}
bool SkeletonRenderer::getDebugBonesEnabled () const {
	return _debugBones;
}

void SkeletonRenderer::setOpacityModifyRGB (bool value) {
	_premultipliedAlpha = value;
}

bool SkeletonRenderer::isOpacityModifyRGB () const {
	return _premultipliedAlpha;
}

}
