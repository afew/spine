
#include <jni.h>
#include <android/log.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include <stdio.h>
#include <stdlib.h>

#include "app_util.h"
#include "ogl_util.h"

extern void app_key_event(int index, int action);
extern void app_touch_event(int index, int action, float x, float y);
extern int app_init();
extern int app_destroy();
extern int app_update();
extern int app_draw();
extern void setAAssetManager(void* assm);

bool setupGraphics(int w, int h)
{
	LOGI("glViewport: %d %d", w, h);
	glViewport(0, 0, w, h);
	return true;
}

extern void setAAssetManager(void* assm);
extern "C" {
	JNIEXPORT void JNICALL Java_com_glc_glbase_MainActivity_keyEvent    (JNIEnv* env, jobject obj, jint index, jint action);
	JNIEXPORT void JNICALL Java_com_glc_glbase_MainActivity_touchEvent  (JNIEnv* env, jobject obj, jint index, jint action, jfloat x, jfloat y);

	JNIEXPORT void JNICALL Java_com_glc_glbase_MainActivity_init        (JNIEnv * env, jobject obj, jobject assetManager, jint width, jint height);
	JNIEXPORT void JNICALL Java_com_glc_glbase_MainActivity_destroy     (JNIEnv * env, jobject obj);
	JNIEXPORT void JNICALL Java_com_glc_glbase_MainActivity_update      (JNIEnv * env, jobject obj);
	JNIEXPORT void JNICALL Java_com_glc_glbase_MainActivity_draw        (JNIEnv * env, jobject obj);

};
void Java_com_glc_glbase_MainActivity_keyEvent(JNIEnv* env, jobject obj, jint index, jint action)
{
	app_key_event(index, action);
}

void Java_com_glc_glbase_MainActivity_touchEvent(JNIEnv* env, jobject obj, jint index, jint action, jfloat x, jfloat y)
{
	app_touch_event(index, action, x, y);
}

void Java_com_glc_glbase_MainActivity_init(JNIEnv * env, jobject obj, jobject assetManager,  jint width, jint height)
{
	setAAssetManager(AAssetManager_fromJava(env, assetManager) );
	setupGraphics(width, height);
	app_init();
}

void Java_com_glc_glbase_MainActivity_destroy(JNIEnv * env, jobject obj)
{
	app_destroy();
}

void Java_com_glc_glbase_MainActivity_update(JNIEnv * env, jobject obj)
{
	app_update();
}

void Java_com_glc_glbase_MainActivity_draw(JNIEnv * env, jobject obj)
{
	app_draw();
}

