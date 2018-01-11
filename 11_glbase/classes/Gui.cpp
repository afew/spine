﻿
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
#include "Gui.h"

#include "LcMath.h"

GuiObject* Gui::createButton(const char* file_name0, const char* file_name1)
{
	Gui* ret = new Gui;
	if(0>ret->Init(file_name0, file_name1))
	{
		delete ret;
		return NULL;
	}
	return ret;
}

Gui::Gui()
	: m_prg(0)
	, m_tx0(0)
	, m_tx1(0)
{
}

Gui::~Gui()
{
}

int Gui::Init(CPVOID p0, CPVOID p1, CPVOID, CPVOID)
{
	const char* tex0 = (const char*)p0;
	const char* tex1 = (const char*)p1;

	m_tx0= GLTexture::createFromFile(tex0);
	if(!m_tx0)
		return -1;
	m_tx1= GLTexture::createFromFile(tex1);
	if(!m_tx1)
		return -1;

	m_prg = GLProgram::createFromFile("media/shader/gui.vert", "media/shader/gui.frag");
	if(!m_prg)
		return -1;

	return 0;
}

int Gui::Destroy()
{
	SAFE_DELETE(m_prg);
	SAFE_DELETE(m_tx0);
	SAFE_DELETE(m_tx1);
	return 0;
}

int Gui::FrameMove()
{
	return 0;
}

int Gui::Render()
{
	// Setting Rendering pipeline
	glDisable( GL_DEPTH_TEST);
	glDisable( GL_CULL_FACE );

	GLCamera* cam = GLCamera::globalCamera("gui");
	if(!cam)
		return -1;

	LCXVEC3 src_pos[] =
	{
		LCXVEC3(-64.0F, -32.0F, 0.0F),
		LCXVEC3(+64.0F, -32.0F, 0.0F),
		LCXVEC3(+64.0F, +32.0F, 0.0F),
		LCXVEC3(-64.0F, +32.0F, 0.0F),
	};
	COLOR4F Dif[] =
	{
		COLOR4F::WHITE,
		COLOR4F::WHITE,
		COLOR4F::WHITE,
		COLOR4F::WHITE,
	};
	LCXVEC2 Tex[] =
	{
		LCXVEC2(0.0F, 0.0F),
		LCXVEC2(1.0F, 0.0F),
		LCXVEC2(1.0F, 1.0F),
		LCXVEC2(0.0F, 1.0F),
	};

	//LCXVEC3 Pos[4];
	//LCXMATRIX matProj = *cam->Proj();
	//for(int i=0; i<4; ++i)
	//{
	//	LCXVEC3 tmp = src_pos[i];
	//	matProj.TransformCoord(&Pos[i], &tmp);
	//}


	m_prg->BeginProgram();

	LCXMATRIX	mtWld;




	m_prg->Texture("us_tx0", 0, m_tx0);
	m_prg->Texture("us_tx1", 1, m_tx1);

	m_prg->Matrix16("um_Prj", (GLfloat*)cam->Proj());


	glEnableVertexAttribArray(0);	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, &src_pos[0]);
	glEnableVertexAttribArray(1);	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, &Dif[0]);
	glEnableVertexAttribArray(2);	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, &Tex[0]);

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(1);
	m_prg->EndProgram();

	return 0;
}

