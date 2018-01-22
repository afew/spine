#ifdef WIN32
  #pragma once
#endif
#ifndef _LcxSpineEx_H_
#define _LcxSpineEx_H_

#include "ogl_util.h"

#include <spine/spine.h>
#include <spine/extension.h>

struct SpineMesh
{
	void*			texture;
	float*			pos;
	float*			dif;
	float*			tex;
	int				stride;
	int				vtx_num;
	unsigned short*	idx_buf;
	int				idx_num;
};

class LcxSpineEx : public RenderObject
{
protected:
	spAtlas*				m_atlas		{};
	spSkeletonData*			m_skeletonData{};
	spAnimationStateData*	m_animationStateData{};
	SpineMesh			m_mesh		{};
	spSkeleton*			m_skeleton	{};
	spAnimationState*	m_state		{};
	float			m_timeScale {1.0F};

	LC_RECT	m_clipping {200,20,900, 560};

public:
	LcxSpineEx();
	virtual ~LcxSpineEx();
	static LcxSpineEx* create(const char* skel, const char* atlas);

	virtual int Init      (CPVOID c_skel=NULL, CPVOID c_atlas=NULL, CPVOID =NULL, CPVOID =NULL) override;
	virtual int Destroy   () override;
	virtual int FrameMove () override;
	virtual int Render    () override;
};

#endif
