#ifdef WIN32
  #pragma once
#endif
#ifndef _LcxSpine_H_
#define _LcxSpine_H_

#include "ogl_util.h"

void init_spine_lib();

class LcxSpine : public RenderObject
{
protected:
	void* m_spineSkeleton {};
	void* m_currentTrack  {};
	LCXTRANSFORM2D	m_tm;
	LC_RECT	m_clipping {200,20,900, 560};

public:
	LcxSpine();
	virtual ~LcxSpine();
	static LcxSpine* create(const char* skel, const char* atlas);
	RenderObject* clone () override;

	virtual int Init      (CPVOID c_skel=NULL, CPVOID c_atlas=NULL, CPVOID =NULL, CPVOID =NULL) override;
	virtual int Destroy   () override;
	virtual int FrameMove () override;
	virtual int Render    () override;

	virtual void Position (const LCXVEC3&) override;
	virtual void Scaling  (const LCXVEC3&) override;
	virtual void Rotation (const float radian) override;
};

#endif
