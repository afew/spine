#pragma once

#ifndef _Sphere_H_
#define _Sphere_H_

#include "ogl_util.h"

class Sphere : public RenderObject
{
protected:
	GLProgram*	m_prg		{};
	GLTexture*	m_tex		{};		// diffuse
	GLMesh		mesh_spine	{};

public:
	Sphere();
	virtual ~Sphere();

	virtual int Init      (CPVOID =NULL, CPVOID =NULL, CPVOID =NULL, CPVOID =NULL) override;
	virtual int Destroy   () override;
	virtual int FrameMove () override;
	virtual int Render    () override;
};

#endif
