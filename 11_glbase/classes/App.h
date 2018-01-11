#pragma once

#ifndef _App_H_
#define _App_H_

#include "ogl_util.h"
#include "Gui.h"
#include <spine/SkeletonAnimation.h>
using namespace spine;

class App : public RenderObject
{
protected:
	GLFBO*			m_fbo    {};
	RenderObject*	m_spine  {};
	GuiObject*		m_button {};
	GLCamera*		m_cam3d  {};
	GLCamera*		m_cam_gui{};

	SkeletonAnimation* m_spineSkeleton {};
	GLProgram*		spine_prg {};


public:
	static App* getInstance();
	App();
	virtual ~App();

	virtual int Init      (CPVOID =NULL, CPVOID =NULL, CPVOID =NULL, CPVOID =NULL) override;
	virtual int Destroy   () override;
	virtual int FrameMove () override;
	virtual int Render    () override;
};

#endif
