
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

static Spine* g_app = NULL;

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


	// 버텍스 생성
	int	iNSphereSegments	= 64;
	sphere_triangle = 2*iNSphereSegments*(iNSphereSegments+1);

	float delta = float( LCX_PI/ iNSphereSegments );

	LCXVEC3*  sphere_pos = new LCXVEC3[sphere_triangle]{};
	LCXVEC3*  sphere_nor = new LCXVEC3[sphere_triangle]{};
	COLOR32*  sphere_dif = new COLOR32[sphere_triangle]{};
	LCXVEC2*  sphere_tex = new LCXVEC2[sphere_triangle]{};

	int index=0;
	for(int theta = 0; theta < iNSphereSegments; theta++ )
	{
		float y0 = 1.0F * cosf( (theta+0) * delta );
		float r0 = 1.0F * sinf( (theta+0) * delta );
		float y1 = 1.0F * cosf( (theta+1) * delta );
		float r1 = 1.0F * sinf( (theta+1) * delta );

		for(int phi = 0; phi < (iNSphereSegments+1); phi++ )
		{
			float x0 =  r0 * sinf( phi * delta * 2.0F );
			float z0 =  r0 * cosf( phi * delta * 2.0F );
			float x1 =  r1 * sinf( phi * delta * 2.0F );
			float z1 =  r1 * cosf( phi * delta * 2.0F );

			sphere_pos[index] = LCXVEC3(x0, y0, z0);
			sphere_nor[index] = sphere_pos[index];
			sphere_dif[index] = LCXCOLOR(1.0F, 1.0F, 1.0F, y0);
			sphere_tex[index] = LCXVEC2(phi/float(iNSphereSegments+.0F), 1.0F-(theta +0.0F)/iNSphereSegments);
			++index;

			sphere_pos[index] = LCXVEC3(x1, y1, z1);
			sphere_nor[index] = sphere_pos[index];
			sphere_dif[index] = LCXCOLOR(1.0F, 1.0F, 1.0F, y1);
			sphere_tex[index] = LCXVEC2(phi/float(iNSphereSegments+.0F), 1.0F-(theta +1.0F)/iNSphereSegments);
			++index;
		}

	}

	glGenBuffers(4, &mesh_sphere.pos);
	glBindBuffer(GL_ARRAY_BUFFER, mesh_sphere.pos);	glBufferData(GL_ARRAY_BUFFER, sizeof(LCXVEC3) *sphere_triangle, sphere_pos, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, mesh_sphere.nor);	glBufferData(GL_ARRAY_BUFFER, sizeof(LCXVEC3) *sphere_triangle, sphere_nor, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, mesh_sphere.dif);	glBufferData(GL_ARRAY_BUFFER, sizeof(COLOR32) *sphere_triangle, sphere_dif, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, mesh_sphere.tex);	glBufferData(GL_ARRAY_BUFFER, sizeof(LCXVEC2) *sphere_triangle, sphere_tex, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	SAFE_DELETE_ARRAY(sphere_pos);
	SAFE_DELETE_ARRAY(sphere_nor);
	SAFE_DELETE_ARRAY(sphere_dif);
	SAFE_DELETE_ARRAY(sphere_tex);

	return 0;
}

int Spine::Destroy()
{
	SAFE_DELETE(m_prg);

	if(mesh_sphere.pos)
	{
		glDeleteBuffers(4, &mesh_sphere.pos);
		mesh_sphere.pos =0;
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

	glEnableVertexAttribArray(0);	glBindBuffer(GL_ARRAY_BUFFER, mesh_sphere.pos);	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);	glBindBuffer(GL_ARRAY_BUFFER, mesh_sphere.nor);	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);	glBindBuffer(GL_ARRAY_BUFFER, mesh_sphere.dif);	glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, 0);
	glEnableVertexAttribArray(3);	glBindBuffer(GL_ARRAY_BUFFER, mesh_sphere.tex);	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, sphere_triangle - 2);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glEnableVertexAttribArray(3);	glBindBuffer(GL_ARRAY_BUFFER, 3);	glDisableVertexAttribArray(3);
	glEnableVertexAttribArray(2);	glBindBuffer(GL_ARRAY_BUFFER, 2);	glDisableVertexAttribArray(2);
	glEnableVertexAttribArray(1);	glBindBuffer(GL_ARRAY_BUFFER, 1);	glDisableVertexAttribArray(1);
	glEnableVertexAttribArray(0);	glBindBuffer(GL_ARRAY_BUFFER, 0);	glDisableVertexAttribArray(0);

	m_prg->EndProgram();

	return LC_OK;
}

