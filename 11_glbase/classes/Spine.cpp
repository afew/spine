
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

Spine::Spine()
{
}

Spine::~Spine()
{
	Destroy();
}

int Spine::Init(CPVOID, CPVOID, CPVOID, CPVOID)
{
	m_tex= GLTexture::createFromFile("media/texture/earth.tga", GLTexture::TYPE_2D);
	if(!m_tex)
		return -1;

	m_prg = GLProgram::createFromFile("media/shader/simple3d.vert", "media/shader/simple3d.frag",{ "at_pos", "at_nor", "at_dif", "at_tex"});
	if(!m_prg)
		return -1;

	return 0;
}

int Spine::Destroy()
{
	SAFE_DELETE(m_prg);

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

