
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
#include "Spine.h"


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

	spine_prg = GLProgram::createFromFile("media/shader/spine.vert", "media/shader/spine.frag");
	if(!spine_prg)
		return -1;

	m_spineSkeleton = spineSkeleton;
	return 0;
}

int Spine::Destroy()
{
	SAFE_DELETE(spine_prg);
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
	SkeletonAnimation* spineSkeleton = (SkeletonAnimation*)m_spineSkeleton;
	if(spineSkeleton)
		spineSkeleton->draw();

	return LC_OK;
}

