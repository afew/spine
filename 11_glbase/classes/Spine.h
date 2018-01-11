#pragma once

#ifndef _Spine_H_
#define _Spine_H_

#include "ogl_util.h"

#define SPINE_SHORT_NAMES
#include <spine/spine.h>
#include <spine/extension.h>

#include <vector>


class Spine : public RenderObject
{
protected:
	GLProgram*		m_prg		{};
	GLTexture*		m_tex		{};
	GLMesh		mesh_spine	{};


public:
	Spine();
	virtual ~Spine();

	virtual int Init      (CPVOID =NULL, CPVOID =NULL, CPVOID =NULL, CPVOID =NULL) override;
	virtual int Destroy   () override;
	virtual int FrameMove () override;
	virtual int Render    () override;
};

#endif
