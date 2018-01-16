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

#include "ogl_util.h"
#include "app_util.h"
#if defined(__APPLE__)
  #define GL_GLEXT_PROTOTYPES
  #import <OpenGLES/ES2/glext.h>
#else
  #include <GLES2/gl2.h>
#endif
#include <spine/extension.h>
#include "spine-cocos2dx.h"

static spFunc_createTexture  _func_createTexture;
static spFunc_releaseTexture _func_releaseTexture;
static spFunc_readFile       _func_readFile;
static spFunc_drawPrimitive  _func_drawPrimitive;

void spine_functor(   spFunc_createTexture  _createTexture
					, spFunc_releaseTexture _releaseTexture
					, spFunc_readFile       _readFile
					, spFunc_drawPrimitive  _drawPrimitive
					) {
	_func_createTexture  = _createTexture ;
	_func_releaseTexture = _releaseTexture;
	_func_readFile       = _readFile      ;
	_func_drawPrimitive  = _drawPrimitive ;
}

static GLuint wrap (spAtlasWrap wrap) {
	return wrap == SP_ATLAS_CLAMPTOEDGE ? GL_CLAMP_TO_EDGE : GL_REPEAT;
}

static GLuint filter (spAtlasFilter filter) {
	switch (filter) {
		case SP_ATLAS_NEAREST:					return GL_NEAREST;
		case SP_ATLAS_LINEAR:					return GL_LINEAR;
		case SP_ATLAS_MIPMAP:					return GL_LINEAR_MIPMAP_LINEAR;
		case SP_ATLAS_MIPMAP_NEAREST_NEAREST:	return GL_NEAREST_MIPMAP_NEAREST;
		case SP_ATLAS_MIPMAP_LINEAR_NEAREST:	return GL_LINEAR_MIPMAP_NEAREST;
		case SP_ATLAS_MIPMAP_NEAREST_LINEAR:	return GL_NEAREST_MIPMAP_LINEAR;
		case SP_ATLAS_MIPMAP_LINEAR_LINEAR:		return GL_LINEAR_MIPMAP_LINEAR;
		default:	break;
	}
	return GL_LINEAR;
}

void _spAtlasPage_createTexture (spAtlasPage* self, const char* path) {
	//GLTexture* texture = GLTexture::createFromFile(path, GLTexture::TYPE_2D, filter(self->minFilter), filter(self->magFilter), wrap(self->uWrap), wrap(self->vWrap));
	//if(!texture)
	//	return;
	//self->rendererObject = texture;
	//self->width  = texture->ImageW();
	//self->height = texture->ImageH();
	if(!_func_createTexture)
		return;
	int width = 0;
	int height= 0;
	void* texture = _func_createTexture(&width, &height, path, filter(self->minFilter), filter(self->magFilter), wrap(self->uWrap), wrap(self->vWrap));
	if(!texture)
		return;
	self->rendererObject = texture;
	self->width  = width;
	self->height = height;
}

void _spAtlasPage_disposeTexture (spAtlasPage* self) {
	//GLTexture* texture = (GLTexture*)(self->rendererObject);
	//if(texture) {
	//	delete texture;
	//	self->rendererObject = NULL;
	//}
	if(!_func_releaseTexture)
		return;
	_func_releaseTexture(self->rendererObject);
	self->rendererObject = NULL;
}

char* _spUtil_readFile (const char* path, int* length) {
	char* ret_buf = NULL;
	int   ret_len = 0;
	int   ret = FileData::getFileDataWithMalloc(&ret_buf, &ret_len, path);
	if(0>ret)
		return NULL;

	*length = ret_len;
	return ret_buf;
}

void spUtil_drawPrimitive(
			const void* _texture,
			const float* vertices, const float* colors, const float* texCoords, int stride,
			const unsigned short* idx_buf, int idx_count
			) {
	if(!_func_drawPrimitive)
		return;
	_func_drawPrimitive(_texture, vertices, colors, texCoords, stride, idx_buf, idx_count);
}
