
#include <algorithm>
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
	const char* file_skel  = (const char*)c_skel;
	const char* file_atlas = (const char*)c_atlas;
	std::string str_file(file_skel);
	std::string str_ext = str_file.substr(str_file.find_last_of("."));
	std::transform(str_ext.begin(), str_ext.end(), str_ext.begin(), ::tolower);
	const char* is_json = strstr(str_ext.c_str(), ".json");

	SkeletonAnimation* spineSkeleton{};
	if(is_json)
		spineSkeleton = SkeletonAnimation::createWithJsonFile(str_file, std::string(file_atlas), 1.0F);
	else
		spineSkeleton = SkeletonAnimation::createWithBinaryFile(str_file, std::string(file_atlas), 1.0F);
	if(!spineSkeleton)
		return LC_EFAIL;
	m_currentTrack = spineSkeleton->setAnimation(0, 0, true);
	m_spineSkeleton = spineSkeleton;
	return LC_OK;
}

int LcxSpine::Destroy()
{
	SkeletonAnimation* spineSkeleton = (SkeletonAnimation*)m_spineSkeleton;
	if(spineSkeleton)
	{
		delete spineSkeleton;
		m_spineSkeleton = NULL;
	}

	return LC_OK;
}

int LcxSpine::FrameMove()
{
	SkeletonAnimation* spineSkeleton = (SkeletonAnimation*)m_spineSkeleton;
	if(spineSkeleton)
		spineSkeleton->update(0.02f);
	return LC_OK;
}

int	LcxSpine::Render()
{
	SkeletonAnimation* spineSkeleton = (SkeletonAnimation*)m_spineSkeleton;
	if(!spineSkeleton)
		return LC_EFAIL;

	int     stored_scissor_test = glIsEnabled(GL_SCISSOR_TEST);
	LC_RECT	stored_scissor_box;
	glGetIntegerv(GL_SCISSOR_BOX, (int*)&stored_scissor_box);
	glEnable(GL_SCISSOR_TEST);
	glScissor(m_clipping.x, m_clipping.y, m_clipping.w, m_clipping.h);

	GLCamera* cam = GLCamera::globalCamera("gui");
	spine_prg = GLProgram::createFromFile("media/shader/spine.vert", "media/shader/spine.frag");
	if(!spine_prg)
		return LC_EFAIL;


	MAT4X4 tm_wld;
	tm_wld.Scaling(1.0F, 1.0F, 1.0F);
	tm_wld._41 = -600;
	tm_wld._42 = -320;

	spine_wld = tm_wld;
	spine_viw = *cam->View();
	spine_prj = *cam->Proj();
	LCXMATRIX  tm_WVP = spine_wld * spine_viw * spine_prj;

	LCXCOLOR us_tc{1.0F, 1.0F, 1.0F, 1.0F};

	spineSkeleton->draw([this, &us_tc, &tm_WVP](const spine::SpineMeshArgs& args)
	{
		//GLTexture* texture      = (GLTexture*)std::get<SPINEMESHARGS_TEXTURE>(args);
		//const float* pos        = std::get<SPINEMESHARGS_POSITION>(args);
		//const float* dif        = std::get<SPINEMESHARGS_DIFFUSE >(args);
		//const float* tex        = std::get<SPINEMESHARGS_TEXCOORD>(args);
		//int stride              = std::get<SPINEMESHARGS_STRIDE  >(args);
		//const USHORT* idx_buf   = std::get<SPINEMESHARGS_IDX_BUF >(args);
		//int   idx_num           = std::get<SPINEMESHARGS_IDX_NUM >(args);
		GLTexture* texture      = (GLTexture*)args.texture;
		const float* pos        = args.pos;
		const float* dif        = args.dif;
		const float* tex        = args.tex;
		int stride              = args.stride;
		const USHORT* idx_buf   = args.idx_buf;
		int   idx_num           = args.idx_num;

		spine_prg->BeginProgram();
		spine_prg->Matrix16("um_WVP", (float*)&tm_WVP);
		spine_prg->Color4  ("us_tc", (const float*)&us_tc);
		spine_prg->Texture ("us_tx0", 0, texture);


		glEnableVertexAttribArray(0);	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, pos);
		glEnableVertexAttribArray(1);	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride, dif);
		glEnableVertexAttribArray(2);	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, tex);

		glDrawElements(GL_TRIANGLES, idx_num, GL_UNSIGNED_SHORT, idx_buf);
		glDisableVertexAttribArray(2);
		glDisableVertexAttribArray(1);

		spine_prg->EndProgram();
	});

	glScissor(stored_scissor_box.x, stored_scissor_box.y, stored_scissor_box.w, stored_scissor_box.h);
	if(!stored_scissor_box)
		glDisable(GL_SCISSOR_TEST);
	return LC_OK;
}

