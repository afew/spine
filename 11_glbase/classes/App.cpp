﻿
#include <mutex>
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
#include "App.h"
#include "Sphere.h"
#include "Spine.h"



static std::mutex g_mutex;

static App* g_app = NULL;

struct Touch{ int a; float x, y; };	// action, position
static unsigned char g_key_event[256]={0};
static unsigned char g_key_cur  [256]={0};
static Touch g_touch_event[32]={0};
static Touch g_touch_cur  [32]={0};

static bool test_touch_rect(int touch_index, int touch_action, const LCXRECT& rc)
{
	const Touch* touch = &g_touch_cur[touch_index];
	if(touch_action != touch->a)
		return false;

	if(	touch->x > rc.x          &&
		touch->x < (rc.x + rc.w) &&
		touch->y > rc.y          &&
		touch->y < (rc.y + rc.h) )
	{
		return true;
	}
	return false;
}

void app_key_event(int index, int action)
{
	std::lock_guard<std::mutex> lock(g_mutex);

	if(0>index || 255<index)
		return;
	g_key_event[index]=action;
}

void app_touch_event(int index, int action, float x, float y)
{
	std::lock_guard<std::mutex> lock(g_mutex);
	if(0>index || 32<index)
		return;

	//LOGI("app_touch_event: index: %3d  action: %2d ", index, action);
	g_touch_event[index].a = action;
	g_touch_event[index].x = x;
	g_touch_event[index].y = y;
}

void app_update_key()
{
	std::lock_guard<std::mutex> lock(g_mutex);

	memcpy(g_key_cur  , g_key_event  , sizeof(g_key_event  ));
	memcpy(g_touch_cur, g_touch_event, sizeof(g_touch_event));

	memset(g_key_event  , 0, sizeof(g_key_event  ));
	memset(g_touch_event, 0, sizeof(g_touch_event));
}

int app_init()
{
	printGLString("Version", GL_VERSION);
	printGLString("Vendor", GL_VENDOR);
	printGLString("Renderer", GL_RENDERER);
	printGLString("Extensions", GL_EXTENSIONS);
	LOGI("----------------------------------------------------------------------");

	return App::getInstance()->Init();
}

int app_destroy()
{
	if(g_app)
	{
		return App::getInstance()->Destroy();
		delete g_app;
		g_app = NULL;
	}
	return 0;
}

int app_update()
{
	app_update_key();
	return App::getInstance()->FrameMove();
}

int app_draw()
{
	return App::getInstance()->Render();
}

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------

App* App::getInstance()
{
	if(!g_app)
		g_app = new App;
	return g_app;
}

App::App()
	: m_fbo  {}
	, m_spine {}
	, m_button{}
	, m_cam3d{}
	, m_cam_gui{}
{
}


App::~App()
{
}

int App::Init(CPVOID, CPVOID, CPVOID, CPVOID)
{
	m_cam3d = GLCamera::create(GLCamera::GLCAM_3D, "3d world");
	if(!m_cam3d)
		return -1;
	m_cam_gui = GLCamera::create(GLCamera::GLCAM_GUI, "gui");
	if(!m_cam_gui)
		return -1;

	m_button = Gui::createButton("media/texture/button.tga", "media/texture/white.tga");
	if(!m_button)
		return -1;

	int vpt[8]={0};
	glGetIntegerv(GL_VIEWPORT, vpt);
	m_fbo = GLFBO::create(vpt[2], vpt[3]);

	m_spineSkeleton = SkeletonAnimation::createWithJsonFile(
		std::string("media/spine/spineboy.json"),
		std::string("media/spine/spineboy.atlas"));

	spine_prg = GLProgram::createFromFile("media/shader/spine.vert", "media/shader/spine.frag");
	if(!spine_prg)
		return -1;


	return 0;
}

int App::Destroy()
{
	SAFE_DELETE(m_fbo  );
	SAFE_DELETE(m_spine );
	SAFE_DELETE(m_button);
	SAFE_DELETE(spine_prg);

	GLCamera::remove(&m_cam3d);
	GLCamera::remove(&m_cam_gui);
	return 0;
}

int App::FrameMove()
{
	if(test_touch_rect(0, 1, LCXRECT(0,0, 300, 200)))
	{
		LOGI("App::FrameMove() touch -------------------------------------------------------");
	}
	m_cam3d->FrameMove();
	m_button->FrameMove();

	if(m_spineSkeleton)
		m_spineSkeleton->update(0.1f);
	return 0;
}

int App::Render()
{
	m_fbo->begin();
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0f, 0.4f, 0.6f, 1.0f);
	glBlendEquation(GL_FUNC_ADD);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	MAT4X4 tm_wld;
	const MAT4X4* tm_viw = m_cam_gui->View();
	const MAT4X4* tm_prj = m_cam_gui->Proj();
	m_spineSkeleton->draw(spine_prg, tm_wld, *tm_viw, *tm_prj);


	glClear(GL_DEPTH_BUFFER_BIT);
	m_button->Render();

	m_fbo->end();
	//glReadPixels(0, 0, 800, 600, GL_RGBA, GL_UNSIGNED_BYTE, m_work_pixel);
	//m_fbo->SetPixel(800, 600, GL_RGBA, GL_UNSIGNED_BYTE, m_work_pixel);

	if(true)
	{
		glClearColor(0.4f, 0.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		m_fbo->draw();
	}
	return 0;
}


