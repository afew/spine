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

#ifndef SPINE_SKELETONTWOCOLORBATCH_H_
#define SPINE_SKELETONTWOCOLORBATCH_H_

#include <spine/spine.h>
#include <vector>

namespace spine {
	struct V3F_C4B_C4B_T2F {
		LCXVEC3 position;
		LCXCOLORB color;
		LCXCOLORB color2;
		LCXVEC2 texCoords;
	};
	
	struct TwoColorTriangles {
		V3F_C4B_C4B_T2F* verts;
		unsigned short* indices;
		int vertCount;
		int indexCount;
	};
	
 //   class SkeletonTwoColorBatch {
 //   public:
 //       static SkeletonTwoColorBatch* getInstance ();

 //       static void destroyInstance ();

 //       void update (float delta);

	//	V3F_C4B_C4B_T2F* allocateVertices(uint32_t numVertices);
	//	void deallocateVertices(uint32_t numVertices);
	//	
	//	unsigned short* allocateIndices(uint32_t numIndices);
	//	void deallocateIndices(uint32_t numIndices);

	//	TwoColorTrianglesCommand* addCommand(float globalOrder, GLuint textureID, cocos2d::GLProgramState* glProgramState, BlendFunc blendType, const TwoColorTriangles& triangles, const MAT4X4& mv, uint32_t flags);

	//	cocos2d::GLProgramState* getTwoColorTintProgramState () { return _twoColorTintShaderState; }
	//	
	//	void batch (TwoColorTrianglesCommand* command);
	//	
	//	void flush (TwoColorTrianglesCommand* materialCommand);
	//	
	//	uint32_t getNumBatches () { return _numBatches; };
	//	
 //   protected:
 //       SkeletonTwoColorBatch ();
 //       virtual ~SkeletonTwoColorBatch ();

	//	void reset ();

	//	TwoColorTrianglesCommand* nextFreeCommand ();

	//	// pool of commands
	//	std::vector<TwoColorTrianglesCommand*> _commandsPool;
	//	uint32_t _nextFreeCommand;

	//	// pool of vertices
	//	std::vector<V3F_C4B_C4B_T2F> _vertices;
	//	uint32_t _numVertices;
	//	
	//	// pool of indices
	//	spUnsignedShortArray* _indices;
	//	
	//	// two color tint shader and state
	//	GLProgram* _twoColorTintShader;
	//	cocos2d::GLProgramState* _twoColorTintShaderState;
	//	
	//	// VBO handles & attribute locations
	//	GLuint _vertexBufferHandle;
	//	V3F_C4B_C4B_T2F* _vertexBuffer;
	//	uint32_t _numVerticesBuffer;
	//	GLuint _indexBufferHandle;
	//	uint32_t _numIndicesBuffer;
	//	unsigned short* _indexBuffer;
	//	GLint _positionAttributeLocation;
	//	GLint _colorAttributeLocation;
	//	GLint _color2AttributeLocation;
	//	GLint _texCoordsAttributeLocation;
	//	
	//	// last batched command, needed for flushing to set material
	//	TwoColorTrianglesCommand* _lastCommand;
	//	
	//	// number of batches in the last frame
	//	uint32_t _numBatches;
	//};
}

#endif // SPINE_SKELETONTWOCOLORBATCH_H_
