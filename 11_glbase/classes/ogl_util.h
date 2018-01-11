﻿#pragma once

#ifndef _OGL_UTIL_H_
#define _OGL_UTIL_H_

#include <vector>
#include <string>
#include <map>
#include "LcType.h"
#include "LcMath.h"

#ifndef GL_LINEAR
#define GL_LINEAR                         0x2601
#endif
#ifndef GL_CLAMP_TO_EDGE
#define GL_CLAMP_TO_EDGE                  0x812F
#endif

void printGLString(const char *name, int s);
bool checkGLError(const char* functionLastCalled);

struct RenderObject
{
	virtual ~RenderObject(){};
	virtual int Init      (CPVOID =NULL, CPVOID =NULL, CPVOID =NULL, CPVOID =NULL) =0;
	virtual int Destroy   () =0;
	virtual int FrameMove () =0;
	virtual int Render    () =0;
};

class GLTexture
{
public:
	enum
	{
		TYPE_NONE = 0,
		TYPE_2D   = 2,
		TYPE_3D   = 3,
		TYPE_CUBE = 6,
	};
protected:
	int m_type      {TYPE_2D};
	int m_tex       {};
	int m_filter_min{GL_LINEAR};
	int m_filter_mag{GL_LINEAR};
	int m_wrap_s    {GL_CLAMP_TO_EDGE};
	int m_wrap_t    {GL_CLAMP_TO_EDGE};
	int m_imgW      {};
	int m_imgH      {};
	int m_imgD      {};
public:
	static GLTexture* createFromFile(const char* file_name, int type=TYPE_2D, int filterMin = GL_LINEAR, int filterMag = GL_LINEAR, int wrapModeS = GL_CLAMP_TO_EDGE, int wrapModeT = GL_CLAMP_TO_EDGE);

public:
	GLTexture();
	virtual ~GLTexture();

	int  Init2D(const char* file_name, int filterMin = GL_LINEAR, int filterMag = GL_LINEAR, int wrapModeS = GL_CLAMP_TO_EDGE, int wrapModeT = GL_CLAMP_TO_EDGE);
	int  InitCube(const char* file_name, int filterMin = GL_LINEAR, int filterMag = GL_LINEAR, int wrapModeS = GL_CLAMP_TO_EDGE, int wrapModeT = GL_CLAMP_TO_EDGE);
	void Destroy();
	void BindStage   (int stage, int filterMin =0, int filterMag =0, int wrapModeS =0, int wrapModeT =0);
	void UnBindStage (int stage);
	int  SetPixel    (int w, int h, int f, int t, void* pxl);
	int  TexType     () const { return m_type; }
	int  TexID       () const { return m_tex; }

	int ImageW() { return m_imgW; }
	int ImageH() { return m_imgH; }
	int ImageD() { return m_imgD; }
};

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

class GLProgram
{
protected:
	UINT m_vert;
	UINT m_frag;
	UINT m_prog;
	std::map<int, GLTexture*>	m_texture;

public:
	static GLProgram* createFromFile(const char* file_vertexShaderSource, const char* file_fragmentShaderSource, const std::vector<std::string>& ls_attr={std::string("at_pos")});
	static GLProgram* createFromBuffer(const char* vertexShaderSource, const char* fragmentShaderSource, const std::vector<std::string>& ls_attr={std::string("at_pos")});

public:
	GLProgram();
	virtual ~GLProgram();

	int  Init(const char* vertexShaderSource, const char* fragmentShaderSource, const std::vector<std::string>& ls_attr={std::string("at_pos")});
	int  InitFromFile(const char* vertexShaderFile, const char* fragmentShaderFile, const std::vector<std::string>& ls_attr={std::string("at_pos")});
	void Destroy();
	void BeginProgram();
	void EndProgram();

	int UniformLocation(const char* uniform_name);
	int Mat3x3  (const char* uniform_name, const float* float16);
	int Mat4x4  (const char* uniform_name, const float* float16);
	int Matrix16(const char* uniform_name, const float* float16);
	int Vector4 (const char* uniform_name, const float* float4);
	int Vector3 (const char* uniform_name, const float* float3);
	int Vector2 (const char* uniform_name, const float* float2);
	int Float   (const char* uniform_name, const float v);
	int Int     (const char* uniform_name, const int v);
	int Color4  (const char* uniform_name, const float* float4);
	int Color3  (const char* uniform_name, const float* float3);
	int Texture (const char* uniform_name, int stage);
	int Texture (const char* uniform_name, int stage, GLTexture* tex);
};

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
class GLFBO
{
protected:
	int m_tex = 0;	// texture
	int m_rnd = 0;	// render buffer
	int m_frm = 0;	// frame buffer
	int store_tex = 0;
	int store_rnd = 0;
	int store_frm = 0;
    GLProgram* m_prg ={};
public:
	static GLFBO* create(int width, int height);

	GLFBO(int width=1080, int height=720);
	void begin();
	void end();
	void draw();

protected:
	int Init(int w, int h);
	void Destroy();
};

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

class GLCamera : public RenderObject
{
public:
	enum
	{
		GLCAM_NONE = 0,
		GLCAM_3D   = 1,
		GLCAM_2D   = 2,
		GLCAM_GUI  = 3,

	};
protected:
	MAT4X4	mt_viw;
	MAT4X4	mt_prj;
	LCXVEC3 v3_eye;		// camera position
	LCXVEC3 v3_look;	// camera look at position
	LCXVEC3 v3_up;		// camera up vector;
	bool	b_update;
	std::string m_name;
public:
	GLCamera();
	virtual ~GLCamera(){}

	static GLCamera*    create(int type, const char* name=NULL);
	static void         remove(GLCamera**);
	static void         globalCamera(const std::string& name, GLCamera*);
	static GLCamera*    globalCamera(const std::string& name);

	protected: virtual int Init      (CPVOID =NULL, CPVOID =NULL, CPVOID =NULL, CPVOID =NULL) final;
	protected: virtual int Destroy   () final{ return 0;};
	public   : virtual int FrameMove () override;
	protected: virtual int Render    () final{ return 0;};

public:
	void            Eye    (const LCXVEC3& eye);
	const LCXVEC3*  Eye    () const { return &v3_eye; }
	void            LookAt (const LCXVEC3& lookat);
	const LCXVEC3*  LookAt () const { return &v3_eye; }

	void       Param   (const LCXVEC3& eye, const LCXVEC3& lookat, const LCXVEC3& up);
	void       Up      (const LCXVEC3& up);

	const MAT4X4* View () const { return &mt_viw;}
	const MAT4X4* Proj () const { return &mt_prj;}

	const std::string& name() const { return m_name; }
};

//------------------------------------------------------------------------------

class GLCamera3D : public GLCamera
{
public:
	GLCamera3D();
	virtual ~GLCamera3D(){}
	public   : virtual int FrameMove () override;
};

//------------------------------------------------------------------------------

class GLCamera2D : public GLCamera
{
public:
	GLCamera2D();
	virtual ~GLCamera2D(){}
	public   : virtual int FrameMove () override;
};

//------------------------------------------------------------------------------

class GLCameraGui : public GLCamera
{
public:
	GLCameraGui();
	virtual ~GLCameraGui(){}
	public   : virtual int FrameMove () override;
};

//------------------------------------------------------------------------------

struct GLMesh
{
	UINT pos{}, nor{}, dif{}, tex{}, tzn{};	// position, normal, diffuse, texture id, texture id, tanzent
	UINT idx{};								// index id
	UINT num_ids {};						// number ids
	UINT num_tri {};						// triangle count
};

//struct SPINE_VTX
//{
//	LCXVEC3		pos	{};			// vertices (3F): 12 bytes
//	LCXCOLOR	dif	{};			// colors (4F): 16 bytes
//	LCXVEC2		tex	{};			// tex coords (2F): 8 bytes
//};

struct SPINE_MESH
{
//	SPINE_VTX*	verts       {};			// Vertex data pointer
	LCXVEC3 *	vtx_pos		{};			// vertices (3F)
	LCXCOLOR*	vtx_dif		{};			// vertices (4F)
	LCXVEC2 *	vtx_tex		{};			// vertices (2F)
	USHORT  *	indices     {};			// Index data pointer
	int			vertCount   {};			// The number of vertices
	int			indexCount  {};			// The number of indices
};

struct BlendFunc
{
	UINT src;							// source blend function
	UINT dst;							// destination blend function

	BlendFunc(UINT _src=0x0302, UINT _dst=0x0303): src(_src), dst(_dst){}
	bool operator==(const BlendFunc &a) const
	{
		return src == a.src && dst == a.dst;
	}
	bool operator!=(const BlendFunc &a) const
	{
		return src != a.src || dst != a.dst;
	}
	bool operator<(const BlendFunc &a) const
	{
		return src < a.src || (src == a.src && dst < a.dst);
	}

	static const BlendFunc DISABLE;					// Blending disabled. Uses {GL_ONE, GL_ZERO}
	static const BlendFunc ALPHA_PREMULTIPLIED;		// Blending enabled for textures with Alpha premultiplied. Uses {GL_ONE, GL_ONE_MINUS_SRC_ALPHA}
	static const BlendFunc ALPHA_NON_PREMULTIPLIED;	// Blending enabled for textures with Alpha NON premultiplied. Uses {GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA}
	static const BlendFunc ADDITIVE;				// Enables Additive blending. Uses {GL_SRC_ALPHA, GL_ONE}
};


#endif

