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

#ifndef SPINE_ATTACHMENTVERTICES_H_
#define SPINE_ATTACHMENTVERTICES_H_

#include <tuple>

namespace spine {

#ifndef SPINEMESHARGS
#define SPINEMESHARGS
typedef std::tuple
<
	  void*				// 0: texture
	, float*			// 1: pos
	, float*			// 2: tex
	, float*			// 3: color
	, int				// 4: stride
	, int				// 5: vertex count
	, unsigned short*	// 6: idx_buf
	, int				// 7: idx count
> SpineMeshArgs;

enum
{
	SPINEMESHARGS_TEXTURE = 0,
	SPINEMESHARGS_POSITION,
	SPINEMESHARGS_TEXCOORD,
	SPINEMESHARGS_DIFFUSE ,
	SPINEMESHARGS_STRIDE  ,
	SPINEMESHARGS_VTX_NUM ,
	SPINEMESHARGS_IDX_BUF ,
	SPINEMESHARGS_IDX_NUM ,
};
#endif

class AttachmentVertices {
public:
	AttachmentVertices (void* texture, int verticesCount, unsigned short* triangles, int trianglesCount);
	virtual ~AttachmentVertices ();
	SpineMeshArgs	_mesh {};
};

}

#endif /* SPINE_ATTACHMENTVERTICES_H_ */
