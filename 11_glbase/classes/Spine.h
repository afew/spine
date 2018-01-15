#pragma once

#ifndef _Spine_H_
#define _Spine_H_

#include "ogl_util.h"

class Spine : public RenderObject
{
protected:
	void* m_spineSkeleton {};
	GLProgram* spine_prg {};
public:
	Spine();
	virtual ~Spine();
	static Spine* create(const char* skel, const char* atlas);

	virtual int Init      (CPVOID c_skel=NULL, CPVOID c_atlas=NULL, CPVOID =NULL, CPVOID =NULL) override;
	virtual int Destroy   () override;
	virtual int FrameMove () override;
	virtual int Render    () override;
};

#endif
