
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
#include "spine-cocos2dx.h"
#include "Spine.h"



void* Spine_createTexture(int* width, int* height, const char* file_name, int minFilter, int magFilter, int wrap_s, int wrap_t)
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

static void Spine_drawPrimitive(  const void* _texture
								, const float* vertices, const float* colors, const float* texCoords, int stride
								, const unsigned short* idx_buf, int idx_count)
{
	GLTexture* texture = (GLTexture*)_texture;

	spine_prg->Texture("us_tx0", 0, texture);

	spine_prg->Matrix16("um_Wld", (float*)&spine_wld);
	spine_prg->Matrix16("um_Viw", (float*)&spine_viw);
	spine_prg->Matrix16("um_Prj", (float*)&spine_prj);

	glEnableVertexAttribArray(0);	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, vertices);
	glEnableVertexAttribArray(1);	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride, colors);
	glEnableVertexAttribArray(2);	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, texCoords);

	glDrawElements(GL_TRIANGLES, idx_count, GL_UNSIGNED_SHORT, idx_buf);

	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(1);
}

void init_spine_lib()
{
	spine_functor(Spine_createTexture, Spine_releaseTexture, Spine_readFile, Spine_drawPrimitive);
}


#define SPINE_SHORT_NAMES
#include "SkeletonAnimation.h"
using namespace spine;

Spine* Spine::create(const char* skel, const char* atlas)
{
	Spine* obj = NULL;
	if(!skel || !atlas)
		return NULL;

	obj = new Spine;
	if(0>obj->Init(skel, atlas))
	{
		delete obj;
		return NULL;
	}
	return obj;
}

Spine::Spine()
{
}

Spine::~Spine()
{
	Destroy();
}

int Spine::Init(CPVOID c_skel, CPVOID c_atlas, CPVOID c_binary, CPVOID)
{
	const char* skel  = (const char*)c_skel;
	const char* atlas = (const char*)c_atlas;
	SkeletonAnimation* spineSkeleton = SkeletonAnimation::createWithJsonFile(std::string(skel), std::string(atlas), 1.0F);
	if(!spineSkeleton)
		return -1;

	spineSkeleton->setAnimation(0, "walk", true);

	m_prg = GLProgram::createFromFile("media/shader/spine.vert", "media/shader/spine.frag");
	if(!m_prg)
		return -1;

	m_spineSkeleton = spineSkeleton;
	return 0;
}

int Spine::Destroy()
{
	SAFE_DELETE(m_prg);
	SkeletonAnimation* spineSkeleton = (SkeletonAnimation*)m_spineSkeleton;
	if(spineSkeleton)
	{
		delete spineSkeleton;
		m_spineSkeleton = NULL;
	}

	return 0;
}

int Spine::FrameMove()
{
	SkeletonAnimation* spineSkeleton = (SkeletonAnimation*)m_spineSkeleton;
	if(spineSkeleton)
		spineSkeleton->update(0.02f);
	return 0;
}

int	Spine::Render()
{
	GLCamera* cam = GLCamera::globalCamera("gui");

	MAT4X4 tm_wld;
	tm_wld.Scaling(0.5F, 0.5F, 1.0F);
	tm_wld._41 = -300;
	tm_wld._42 = -300;
	const MAT4X4* tm_viw = cam->View();
	const MAT4X4* tm_prj = cam->Proj();

	spine_prg = this->m_prg;
	spine_wld.Identity();
	spine_viw = *cam->View();
	spine_prj = *cam->Proj();

	SkeletonAnimation* spineSkeleton = (SkeletonAnimation*)m_spineSkeleton;
	if(spineSkeleton)
		spineSkeleton->draw();

	return LC_OK;
}

