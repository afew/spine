
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

//static SkeletonData* readSkeletonJsonData(const char* filename, Atlas* atlas, float scale)
//{
//	SkeletonJson* json = SkeletonJson_create(atlas);
//	json->scale = scale;
//	SkeletonData* ret = SkeletonJson_readSkeletonDataFile(json, filename);
//	if(!ret) {
//		printf("%s\n", json->error);
//		exit(0);
//	}
//	SkeletonJson_dispose(json);
//	return ret;
//}
//
//static SkeletonData* readSkeletonBinaryData(const char* filename, Atlas* atlas, float scale)
//{
//	SkeletonBinary* binary = SkeletonBinary_create(atlas);
//	binary->scale = scale;
//	SkeletonData *ret = SkeletonBinary_readSkeletonDataFile(binary, filename);
//	if(!ret) {
//		printf("%s\n", binary->error);
//		exit(0);
//	}
//	SkeletonBinary_dispose(binary);
//	return ret;
//}


Spine::Spine()
{
}

Spine::~Spine()
{
	Destroy();
}

int Spine::Init(CPVOID c_atals, CPVOID c_json, CPVOID c_binary, CPVOID)
{
	m_tex= GLTexture::createFromFile("media/texture/earth.tga", GLTexture::TYPE_2D);
	if(!m_tex)
		return -1;

	m_prg = GLProgram::createFromFile("media/shader/simple3d.vert", "media/shader/simple3d.frag",{ "at_pos", "at_nor", "at_dif", "at_tex"});
	if(!m_prg)
		return -1;


	const char* jsonName   = (const char*)c_json;
	const char* binaryName = (const char*)c_binary;
	const char* atlasName  = (const char*)c_atals;
	float scale = 1.0F;

	//m_atlas = Atlas_createFromFile(atlasName, 0);
	//m_skeletonData = readSkeletonJsonData(jsonName, m_atlas, scale);

	//SkeletonBounds* bounds = SkeletonBounds_create();

	//// Configure mixing.
	//AnimationStateData* stateData = AnimationStateData_create(m_skeletonData);
	//AnimationStateData_setMixByName(stateData, "walk", "jump", 0.2f);
	//AnimationStateData_setMixByName(stateData, "jump", "run", 0.2f);


	//drawable->timeScale = 1;

	//Skeleton* skeleton = drawable->skeleton;
	//skeleton->flipX = false;
	//skeleton->flipY = false;
	//Skeleton_setToSetupPose(skeleton);

	//skeleton->x = 320;
	//skeleton->y = 460;
	//Skeleton_updateWorldTransform(skeleton);

	//Slot* headSlot = Skeleton_findSlot(skeleton, "head");


	//AnimationState_addAnimationByName(drawable->state, 0, "walk", true, 0);
	//AnimationState_addAnimationByName(drawable->state, 0, "jump", false, 3);
	//AnimationState_addAnimationByName(drawable->state, 0, "run", true, 0);



	//m_skeletonData = readSkeletonBinaryData(binaryName, m_atlas, scale);
	//func(m_skeletonData, m_atlas);
	//SkeletonData_dispose(m_skeletonData);

	return 0;
}

int Spine::Destroy()
{
	SAFE_DELETE(m_prg);

	//if(m_skeletonData)
	//{
	//	spSkeletonData_dispose(m_skeletonData);
	//	m_skeletonData ={};
	//}
	//if(m_atlas)
	//{
	//	spAtlas_dispose(m_atlas);
	//	m_atlas ={};
	//}



	if(mesh_spine.pos)
	{
		glDeleteBuffers(3, &mesh_spine.pos);
		mesh_spine.pos =0;
	}
	return 0;
}

int Spine::FrameMove()
{
	return 0;
}



int	Spine::Render()
{
	static float c =0;
	c += 0.5;

	// setup the world matrix
	LCXMATRIX	mtWld, mtS, mtZ, mtX, mtY;
	LCXVEC3	pos(0, 0, 0.F);

	mtS.Scaling(4,4,4);
	mtZ.RotationZ( LCXToRadian((float)(0) ));
	mtX.RotationX( LCXToRadian((float)(0) ));
	mtY.RotationY( LCXToRadian((float)(c) ));

	mtWld = mtX* mtY;
	mtWld *= mtS;

	mtWld._41 = pos.x;
	mtWld._42 = pos.y;
	mtWld._43 = pos.z;

	// Setting Rendering pipeline
	glEnable( GL_DEPTH_TEST);
	glEnable( GL_CULL_FACE );


	m_prg->BeginProgram();

	LCXCOLOR color{1.0F, 1.0F, 1.0F, 1.0F};
	m_prg->Color4("uc_dif", (float*)&color);
	m_prg->Texture("us_tx0", 0, m_tex);

	GLCamera* cam = GLCamera::globalCamera("3d world");
	if(cam)
	{
		m_prg->Matrix16("um_Wld", (GLfloat*)&mtWld);
		m_prg->Matrix16("um_Viw", (GLfloat*)cam->View());
		m_prg->Matrix16("um_Prj", (GLfloat*)cam->Proj());
	}


	m_prg->EndProgram();

	return LC_OK;
}

