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

#include "ogl_util.h"
#include <algorithm>

#if defined(__APPLE__)
  #define GL_GLEXT_PROTOTYPES
  #import <OpenGLES/ES2/glext.h>
#else
  #include <GLES2/gl2.h>
#endif

#include <spine/SkeletonRenderer.h>
#include <spine/extension.h>
#include <spine/SkeletonBatch.h>
#include <spine/AttachmentVertices.h>
#include <spine/Cocos2dAttachmentLoader.h>

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
	_worldVertices.resize(INITIAL_WORLD_VRTEX_SIZE); // Max number of vertices per mesh.

	_blendFunc = BlendFunc::ALPHA_PREMULTIPLIED;
	setOpacityModifyRGB(true);
}

void SkeletonRenderer::setSkeletonData (spSkeletonData *skeletonData, bool ownsSkeletonData) {
	_skeleton = spSkeleton_create(skeletonData);
	_ownsSkeletonData = ownsSkeletonData;
}

SkeletonRenderer::SkeletonRenderer ()
	: _atlas(nullptr), _attachmentLoader(nullptr), _debugSlots(false), _debugBones(false), _timeScale(1) {
}

SkeletonRenderer::SkeletonRenderer (spSkeletonData *skeletonData, bool ownsSkeletonData)
	: _atlas(nullptr), _attachmentLoader(nullptr), _debugSlots(false), _debugBones(false), _timeScale(1) {
	initWithData(skeletonData, ownsSkeletonData);
}

SkeletonRenderer::SkeletonRenderer (const std::string& skeletonDataFile, spAtlas* atlas, float scale)
	: _atlas(nullptr), _attachmentLoader(nullptr), _debugSlots(false), _debugBones(false), _timeScale(1) {
	initWithJsonFile(skeletonDataFile, atlas, scale);
}

SkeletonRenderer::SkeletonRenderer (const std::string& skeletonDataFile, const std::string& atlasFile, float scale)
	: _atlas(nullptr), _attachmentLoader(nullptr), _debugSlots(false), _debugBones(false), _timeScale(1) {
	initWithJsonFile(skeletonDataFile, atlasFile, scale);
}

SkeletonRenderer::~SkeletonRenderer () {
	if (_ownsSkeletonData) spSkeletonData_dispose(_skeleton->data);
	spSkeleton_dispose(_skeleton);
	if (_atlas) spAtlas_dispose(_atlas);
	if (_attachmentLoader) spAttachmentLoader_dispose(_attachmentLoader);
	_worldVertices.clear();
}

void SkeletonRenderer::initWithData (spSkeletonData* skeletonData, bool ownsSkeletonData) {
	setSkeletonData(skeletonData, ownsSkeletonData);

	initialize();
}

void SkeletonRenderer::initWithJsonFile (const std::string& skeletonDataFile, spAtlas* atlas, float scale) {
    _atlas = atlas;
	_attachmentLoader = SUPER(Cocos2dAttachmentLoader_create(_atlas));

	spSkeletonJson* json = spSkeletonJson_createWithLoader(_attachmentLoader);
	json->scale = scale;
	spSkeletonData* skeletonData = spSkeletonJson_readSkeletonDataFile(json, skeletonDataFile.c_str());
	if(!skeletonData) {
		//json->error ? json->error : "Error reading skeleton data.");
		return;
	}
	spSkeletonJson_dispose(json);

	setSkeletonData(skeletonData, true);

	initialize();
}

void SkeletonRenderer::initWithJsonFile (const std::string& skeletonDataFile, const std::string& atlasFile, float scale) {
	_atlas = spAtlas_createFromFile(atlasFile.c_str(), 0);
	if(!_atlas) {
		//, "Error reading atlas file.");
		return;
	}

	_attachmentLoader = SUPER(Cocos2dAttachmentLoader_create(_atlas));

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
    _atlas = atlas;
    _attachmentLoader = SUPER(Cocos2dAttachmentLoader_create(_atlas));

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
    _atlas = spAtlas_createFromFile(atlasFile.c_str(), 0);
    if(!_atlas) {
		//, "Error reading atlas file.");
		return;
	}

    _attachmentLoader = SUPER(Cocos2dAttachmentLoader_create(_atlas));

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
	spSkeleton_update(_skeleton, deltaTime * _timeScale);
}

void SkeletonRenderer::draw (GLProgram* prog, const MAT4X4& parentWorld, const MAT4X4& view, const MAT4X4& proj) {

	LCXCOLOR nodeColor = m_color;
	_skeleton->r = nodeColor.r;
	_skeleton->g = nodeColor.g;
	_skeleton->b = nodeColor.b;
	_skeleton->a = nodeColor.a;

    LCXCOLOR color;
	AttachmentVertices* attachmentVertices = nullptr;
	for (int i = 0, n = _skeleton->slotsCount; i < n; ++i) {
		spSlot* slot = _skeleton->drawOrder[i];
		if (!slot->attachment) continue;

		switch (slot->attachment->type) {
		case SP_ATTACHMENT_REGION: {
			spRegionAttachment* attachment = (spRegionAttachment*)slot->attachment;
			spRegionAttachment_computeWorldVertices(attachment, slot->bone, _worldVertices.data());
			attachmentVertices = getAttachmentVertices(attachment);
            color.r = attachment->r;
			color.g = attachment->g;
			color.b = attachment->b;
			color.a = attachment->a;
			break;
		}
		case SP_ATTACHMENT_MESH: {
			spMeshAttachment* attachment = (spMeshAttachment*)slot->attachment;
			int worldVerticesLength = attachment->super.worldVerticesLength;
			if((int)_worldVertices.size() <worldVerticesLength)
				_worldVertices.resize(worldVerticesLength);
			spMeshAttachment_computeWorldVertices(attachment, slot, _worldVertices.data());
			attachmentVertices = getAttachmentVertices(attachment);
            color.r = attachment->r;
            color.g = attachment->g;
            color.b = attachment->b;
            color.a = attachment->a;
			break;
		}
		default:
			continue;
		}

		color.a *= _skeleton->a * slot->a;
		float multiplier = _premultipliedAlpha ? color.a : 1.0F;
		color.r *= _skeleton->r * slot->r * multiplier;
		color.g *= _skeleton->g * slot->g * multiplier;
		color.b *= _skeleton->b * slot->b * multiplier;



		for (int v = 0, w = 0, vn = attachmentVertices->_triangles->vertCount; v < vn; ++v, w += 2) {
			LCXVEC3 * pos = attachmentVertices->_triangles->vtx_pos + v;
			LCXCOLOR* dif = attachmentVertices->_triangles->vtx_dif + v;
			pos->x = _worldVertices[w + 0];
			pos->y = _worldVertices[w + 1];
            dif->r = color.r;
            dif->g = color.g;
            dif->b = color.b;
            dif->a = color.a;
		}

		BlendFunc blendFunc;
		switch (slot->data->blendMode) {
		case SP_BLEND_MODE_ADDITIVE:
			blendFunc.src = _premultipliedAlpha ? GL_ONE : GL_SRC_ALPHA;
			blendFunc.dst = GL_ONE;
			break;
		case SP_BLEND_MODE_MULTIPLY:
			blendFunc.src = GL_DST_COLOR;
			blendFunc.dst = GL_ONE_MINUS_SRC_ALPHA;
			break;
		case SP_BLEND_MODE_SCREEN:
			blendFunc.src = GL_ONE;
			blendFunc.dst = GL_ONE_MINUS_SRC_COLOR;
			break;
		default:
			blendFunc.src = _premultipliedAlpha ? GL_ONE : GL_SRC_ALPHA;
			blendFunc.dst = GL_ONE_MINUS_SRC_ALPHA;
		}

		//glBlendEquation(GL_FUNC_ADD);
		glDisable(GL_BLEND);
		glBlendFunc(blendFunc.src, blendFunc.dst);

		GLTexture* texture = attachmentVertices->_texture;
		SPINE_MESH* mesh = attachmentVertices->_triangles;

		LCXVEC3*  pos = mesh->vtx_pos;
		LCXCOLOR* dif = mesh->vtx_dif;
		LCXVEC2*  tex = mesh->vtx_tex;

		prog->BeginProgram();

		prog->Texture("us_tx0", 0, texture);

		glEnableVertexAttribArray(0);	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, pos);
		glEnableVertexAttribArray(1);	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, dif);
		glEnableVertexAttribArray(2);	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, tex);
		glDrawElements(GL_TRIANGLES, mesh->indexCount, GL_UNSIGNED_SHORT, mesh->indices );
		glDisableVertexAttribArray(2);
		glDisableVertexAttribArray(1);

		prog->EndProgram();
	}

	if (_debugSlots || _debugBones) {
        drawDebug();
	}
}

void SkeletonRenderer::drawDebug () {

/*
    DrawNode* drawNode = DrawNode::create();

    if (_debugSlots) {
        // Slots.
        // DrawPrimitives::setDrawColor4B(0, 0, 255, 255);
        glLineWidth(1);
        Vec2 points[4];
        V3F_C4B_T2F_Quad quad;
        for (int i = 0, n = _skeleton->slotsCount; i < n; i++) {
            spSlot* slot = _skeleton->drawOrder[i];
            if (!slot->attachment || slot->attachment->type != SP_ATTACHMENT_REGION) continue;
            spRegionAttachment* attachment = (spRegionAttachment*)slot->attachment;
            spRegionAttachment_computeWorldVertices(attachment, slot->bone, _worldVertices);
            points[0] = Vec2(_worldVertices[0], _worldVertices[1]);
            points[1] = Vec2(_worldVertices[2], _worldVertices[3]);
            points[2] = Vec2(_worldVertices[4], _worldVertices[5]);
            points[3] = Vec2(_worldVertices[6], _worldVertices[7]);
            drawNode->drawPoly(points, 4, true, LCXCOLOR::BLUE);
        }
    }
    if (_debugBones) {
        // Bone lengths.
        glLineWidth(2);
        for (int i = 0, n = _skeleton->bonesCount; i < n; i++) {
            spBone *bone = _skeleton->bones[i];
            float x = bone->data->length * bone->a + bone->worldX;
            float y = bone->data->length * bone->c + bone->worldY;
            drawNode->drawLine(Vec2(bone->worldX, bone->worldY), Vec2(x, y), LCXCOLOR::RED);
        }
        // Bone origins.
        auto color = LCXCOLOR::BLUE; // Root bone is blue.
        for (int i = 0, n = _skeleton->bonesCount; i < n; i++) {
            spBone *bone = _skeleton->bones[i];
            drawNode->drawPoint(Vec2(bone->worldX, bone->worldY), 4, color);
            if (i == 0) color = LCXCOLOR::GREEN;
        }
    }

    drawNode->draw();
	*/
}

AttachmentVertices* SkeletonRenderer::getAttachmentVertices (spRegionAttachment* attachment) const {
	return (AttachmentVertices*)attachment->rendererObject;
}

AttachmentVertices* SkeletonRenderer::getAttachmentVertices (spMeshAttachment* attachment) const {
	return (AttachmentVertices*)attachment->rendererObject;
}

LCXRECT SkeletonRenderer::getBoundingBox () {
	float minX = FLT_MAX, minY = FLT_MAX, maxX = -FLT_MAX, maxY = -FLT_MAX;
	float scaleX = getScale().x, scaleY = getScale().y;
	for (int i = 0; i < _skeleton->slotsCount; ++i) {
		spSlot* slot = _skeleton->slots[i];
		if (!slot->attachment) continue;
		int verticesCount;
		if (slot->attachment->type == SP_ATTACHMENT_REGION) {
			spRegionAttachment* attachment = (spRegionAttachment*)slot->attachment;
			spRegionAttachment_computeWorldVertices(attachment, slot->bone, _worldVertices.data());
			verticesCount = 8;
		} else if (slot->attachment->type == SP_ATTACHMENT_MESH) {
			spMeshAttachment* mesh = (spMeshAttachment*)slot->attachment;
			int worldVerticesLength = mesh->super.worldVerticesLength;
			if((int)_worldVertices.size() <worldVerticesLength)
				_worldVertices.resize(worldVerticesLength);

			spMeshAttachment_computeWorldVertices(mesh, slot, _worldVertices.data());
			verticesCount = mesh->super.worldVerticesLength;
		} else
			continue;
		for (int ii = 0; ii < verticesCount; ii += 2) {
			float x = _worldVertices[ii] * scaleX, y = _worldVertices[ii + 1] * scaleY;
			minX = min(minX, x);
			minY = min(minY, y);
			maxX = max(maxX, x);
			maxY = max(maxY, y);
		}
	}
	LCXVEC3 position = getPosition();
    if (minX == FLT_MAX) minX = minY = maxX = maxY = 0;
	return LCXRECT(position.x + minX, position.y + minY, maxX - minX, maxY - minY);
}

// --- Convenience methods for Skeleton_* functions.

void SkeletonRenderer::updateWorldTransform () {
	spSkeleton_updateWorldTransform(_skeleton);
}

void SkeletonRenderer::setToSetupPose () {
	spSkeleton_setToSetupPose(_skeleton);
}
void SkeletonRenderer::setBonesToSetupPose () {
	spSkeleton_setBonesToSetupPose(_skeleton);
}
void SkeletonRenderer::setSlotsToSetupPose () {
	spSkeleton_setSlotsToSetupPose(_skeleton);
}

spBone* SkeletonRenderer::findBone (const std::string& boneName) const {
	return spSkeleton_findBone(_skeleton, boneName.c_str());
}

spSlot* SkeletonRenderer::findSlot (const std::string& slotName) const {
	return spSkeleton_findSlot(_skeleton, slotName.c_str());
}

bool SkeletonRenderer::setSkin (const std::string& skinName) {
	return spSkeleton_setSkinByName(_skeleton, skinName.empty() ? 0 : skinName.c_str()) ? true : false;
}
bool SkeletonRenderer::setSkin (const char* skinName) {
	return spSkeleton_setSkinByName(_skeleton, skinName) ? true : false;
}

spAttachment* SkeletonRenderer::getAttachment (const std::string& slotName, const std::string& attachmentName) const {
	return spSkeleton_getAttachmentForSlotName(_skeleton, slotName.c_str(), attachmentName.c_str());
}
bool SkeletonRenderer::setAttachment (const std::string& slotName, const std::string& attachmentName) {
	return spSkeleton_setAttachment(_skeleton, slotName.c_str(), attachmentName.empty() ? 0 : attachmentName.c_str()) ? true : false;
}
bool SkeletonRenderer::setAttachment (const std::string& slotName, const char* attachmentName) {
	return spSkeleton_setAttachment(_skeleton, slotName.c_str(), attachmentName) ? true : false;
}

spSkeleton* SkeletonRenderer::getSkeleton () {
	return _skeleton;
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


// --- CCBlendProtocol

const BlendFunc& SkeletonRenderer::getBlendFunc () const {
	return _blendFunc;
}

void SkeletonRenderer::setBlendFunc (const BlendFunc &blendFunc) {
	_blendFunc = blendFunc;
}

void SkeletonRenderer::setOpacityModifyRGB (bool value) {
	_premultipliedAlpha = value;
}

bool SkeletonRenderer::isOpacityModifyRGB () const {
	return _premultipliedAlpha;
}

}