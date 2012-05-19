#ifndef _ANDROID_MPLAYER_CONTEX_H_
#define _ANDROID_MPLAYER_CONTEXT_H_

#include <pthread.h>
#include <jni.h>

class MPlayerContext
{
public:
    MPlayerContext();
    ~MPlayerContext();

private:
    void pre_init();

private:
    JNIEnv* JavaEnv;
    jclass JavaRendererClass;
    jclass JavaMplayerClass;
    jobject JavaRenderer;
    jobject JavaMplayer;
    jmethodID jnativeConfig;
    jmethodID jnativeAudio;
    jmethodID jnativeDeconfig;
    jmethodID jupdate;
    jmethodID jupdate1;
    jmethodID jupdate_main;
    jmethodID jupdate_main_string;
    jmethodID jstop;
    jobject video;
    pthread_t native_thread;
    const jbyte *str;
    char* command;
    JavaVM *jniVM;
    int arm_vfp;
    int arm_neon;
    int w1,h1;
    jobject temp;
    int i,x;
};

#endif
