#ifndef _ANDROID_MPLAYER_CONTEX_H_
#define _ANDROID_MPLAYER_CONTEXT_H_

#include <time.h>
#include <pthread.h>
#include <jni.h>

class MPlayerContext
{
public:
    MPlayerContext();
    ~MPlayerContext();

    void set_mplayer_running(bool running);
    void vm_attach_thread();

private:
    void pre_init();


public:
    bool m_running;
    int m_flip_count;
    time_t m_play_btime;
    time_t m_play_etime;

    int argc;
    char **argv;

public:
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
