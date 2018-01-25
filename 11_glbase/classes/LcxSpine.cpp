
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

RenderObject* LcxSpine::clone()
{
	//LcxSpine* ret = new LcxSpine;
	//ret->m_spineSkeleton = this->m_spineSkeleton;
	return NULL;
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
	m_currentTrack = spineSkeleton->setAnimation(0, "walk", true);
	m_spineSkeleton = spineSkeleton;

	spine_prg = GLProgram::createFromFile("media/shader/spine.vert", "media/shader/spine.frag");
	if(!spine_prg)
		return LC_EFAIL;
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

	//int     stored_scissor_test = glIsEnabled(GL_SCISSOR_TEST);
	//LC_RECT	stored_scissor_box;
	//glGetIntegerv(GL_SCISSOR_BOX, (int*)&stored_scissor_box);
	//glEnable(GL_SCISSOR_TEST);
	//glScissor(m_clipping.x, m_clipping.y, m_clipping.w, m_clipping.h);

	GLCamera* cam = GLCamera::globalCamera("gui");

	spine_wld = m_tm.TM();
	spine_viw = *cam->View();
	spine_prj = *cam->Proj();

	LCXMATRIX  um_tmWVP = spine_wld * spine_viw * spine_prj;
	LCXCOLOR   us_tc{1.0F, 1.0F, 1.0F, 1.0F};
	GLTexture* ut_tex{};


	spine_prg->BeginProgram();
	spine_prg->Matrix16("um_WVP", (float*)&um_tmWVP);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	spineSkeleton->draw([&](const spine::SpineMeshArgs& args)
	{
		GLTexture* texture      = (GLTexture*)args.texture;
		const float* pos        = args.pos;
		const float* dif        = args.dif;
		const float* tex        = args.tex;
		int stride              = args.stride;
		const USHORT* idx_buf   = args.idx_buf;
		int   idx_num           = args.idx_num;

		spine_prg->Color4  ("us_tc", (const float*)&us_tc);
		if(ut_tex != texture)
		{
			ut_tex = texture;
			spine_prg->Texture ("us_tx0", 0, ut_tex);
		}

		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, pos);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride, dif);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, tex);

		glDrawElements(GL_TRIANGLES, idx_num, GL_UNSIGNED_SHORT, idx_buf);
	});

	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(1);

	spine_prg->EndProgram();


	//glScissor(stored_scissor_box.x, stored_scissor_box.y, stored_scissor_box.w, stored_scissor_box.h);
	//if(!stored_scissor_box)
	//	glDisable(GL_SCISSOR_TEST);
	return LC_OK;
}

void LcxSpine::Position(const LCXVEC3& pos)
{
	m_tm.Translation(pos.x, pos.y);
}

void LcxSpine::Scaling(const LCXVEC3& scl)
{
	m_tm.Scaling(scl.x, scl.y);
}

void LcxSpine::Rotation(const float radian)
{

}

