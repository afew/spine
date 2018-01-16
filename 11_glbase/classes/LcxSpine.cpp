
#include <vector>
#include <stdio.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define GL_GLEXT_PROTOTYPES
#ifdef __APPLE__
#import <OpenGLES/ES2/glext.h>
#else
#include <GLES2/gl2.h>
#endif

#include "app_util.h"
#define SPINE_SHORT_NAMES
#include "SkeletonAnimation.h"
#include "spine-cocos2dx.h"
using namespace spine;

#include "LcxSpine.h"


static void* Spine_createTexture(int* width, int* height, const char* file_name, int minFilter, int magFilter, int wrap_s, int wrap_t)
{
	GLTexture* texture = GLTexture::createFromFile(file_name, GLTexture::TYPE_2D, minFilter, magFilter, wrap_s, wrap_t);
	if(!texture)
		return NULL;
	if(width)  *width = texture->ImageW();
	if(height) *height= texture->ImageH();
	return texture;
}
static void Spine_releaseTexture(void* _texture)
{
	GLTexture* texture = (GLTexture*)(_texture);
	if(texture)
		delete texture;
}
static char* Spine_readFile(int* length, const char* path)
{
	char* ret_buf = NULL;
	int   ret_len = 0;
	int   ret = FileData::getFileDataWithMalloc(&ret_buf, &ret_len, path);
	if(0>ret)
		return NULL;
	if(length)
		*length = ret_len;
	return ret_buf;
}

static GLProgram* spine_prg;
static LCXMATRIX  spine_wld;
static LCXMATRIX  spine_viw;
static LCXMATRIX  spine_prj;

void init_spine_lib()
{
	spine_functor(Spine_createTexture, Spine_releaseTexture, Spine_readFile);
}


LcxSpine* LcxSpine::create(const char* skel, const char* atlas)
{
	LcxSpine* obj = NULL;
	if(!skel || !atlas)
		return NULL;

	obj = new LcxSpine;
	if(0>obj->Init(skel, atlas))
	{
		delete obj;
		return NULL;
	}
	return obj;
}

LcxSpine::LcxSpine()
{
}

LcxSpine::~LcxSpine()
{
	Destroy();
}

int LcxSpine::Init(CPVOID c_skel, CPVOID c_atlas, CPVOID c_binary, CPVOID)
{
	const char* skel  = (const char*)c_skel;
	const char* atlas = (const char*)c_atlas;
	SkeletonAnimation* spineSkeleton = SkeletonAnimation::createWithJsonFile(std::string(skel), std::string(atlas), 1.0F);
	if(!spineSkeleton)
		return -1;

	spineSkeleton->setAnimation(0, "walk", true);
	m_spineSkeleton = spineSkeleton;
	return 0;
}

int LcxSpine::Destroy()
{
	SkeletonAnimation* spineSkeleton = (SkeletonAnimation*)m_spineSkeleton;
	if(spineSkeleton)
	{
		delete spineSkeleton;
		m_spineSkeleton = NULL;
	}

	return 0;
}

int LcxSpine::FrameMove()
{
	SkeletonAnimation* spineSkeleton = (SkeletonAnimation*)m_spineSkeleton;
	if(spineSkeleton)
		spineSkeleton->update(0.02f);
	return 0;
}

int	LcxSpine::Render()
{
	GLCamera* cam = GLCamera::globalCamera("gui");
	spine_prg = GLProgram::createFromFile("media/shader/spine.vert", "media/shader/spine.frag");
	if(!spine_prg)
		return -1;


	MAT4X4 tm_wld;
	tm_wld.Scaling(0.5F, 0.5F, 1.0F);
	tm_wld._41 = -300;
	tm_wld._42 = -300;

	spine_wld = tm_wld;
	spine_viw = *cam->View();
	spine_prj = *cam->Proj();
	LCXMATRIX  tm_WVP = spine_wld * spine_viw * spine_prj;
	LCXCOLOR   us_tc {1.0F, 1.0F, 1.0F, 1.0F};

	SkeletonAnimation* spineSkeleton = (SkeletonAnimation*)m_spineSkeleton;
	if(spineSkeleton)
	{
		spineSkeleton->color({3.0F, 5.0F, 1.0F, 0.3F});

		spineSkeleton->draw([this, &tm_WVP, &us_tc](const void* _texture, const MESH_BUF2D* mesh)
		{
			int    stride    = sizeof(VTX_PD2T);
			float* vertices  = (float*)&mesh->vtx->pos;
			float* colors    = (float*)&mesh->vtx->dif;
			float* texCoords = (float*)&mesh->vtx->tex;
			unsigned short* idx_buf = mesh->idx;
			int             idx_num = mesh->n_idx;

			GLTexture* texture = (GLTexture*)_texture;
			spine_prg->BeginProgram();
			spine_prg->Color4 ("us_tc", (const float*)&us_tc);
			spine_prg->Texture("us_tx0", 0, texture);

			spine_prg->Matrix16("um_WVP", (float*)&tm_WVP);

			glEnableVertexAttribArray(0);	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, vertices );
			glEnableVertexAttribArray(1);	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride, colors   );
			glEnableVertexAttribArray(2);	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, texCoords);

			glDrawElements(GL_TRIANGLES, idx_num, GL_UNSIGNED_SHORT, idx_buf);
			glDisableVertexAttribArray(2);
			glDisableVertexAttribArray(1);

			spine_prg->EndProgram();
		});
	}

	return LC_OK;
}

