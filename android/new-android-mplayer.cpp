#include <stdio.h>
#include <string.h>

#include <jni.h>
#include <android/log.h>
// #include "metadata.h"
// #include <pthread.h>
//#include <tag_c.h>

#include "android-mplayer-context.h"
#include "new-android-mplayer.h"

MPlayerContext *mp_ctx = NULL;


#define qlog( ... ) __android_log_print(ANDROID_LOG_ERROR, "MPlayer", "%s ", __FUNCTION__); \
    __android_log_print(ANDROID_LOG_ERROR, "MPlayer", __VA_ARGS__);

// #define qlog( ... )  do {} while (0);

extern "C" {
    
    // called from mplayer's c code
    void update_main_string(uint8_t *name)
    {
        qlog("aaaaaa");
        // jstring jstr;
        // (*jniVM)->AttachCurrentThread(jniVM, &JavaEnv, NULL);
        // jstr=(*JavaEnv)->NewStringUTF(JavaEnv,name);
        // (*JavaEnv)->CallVoidMethod( JavaEnv,JavaMplayer, jupdate_main_string,jstr);
        // (*JavaEnv)->DeleteLocalRef(JavaEnv,jstr);

        mp_ctx->vm_attach_thread();
        // android no iconv
        // todo give java module raw byte array
        unsigned char raw_name[100];
        jobject jraw_name;
        jobject jout_raw_name;

        memset(raw_name, 0, sizeof(raw_name));
        strncpy((char*)raw_name, (char*)name, sizeof(raw_name)-1);
        jraw_name = mp_ctx->JavaEnv->NewDirectByteBuffer((char*)raw_name, strlen((char*)raw_name));
        jout_raw_name  = mp_ctx->JavaEnv->NewLocalRef(jraw_name);
        // output to java peer
        mp_ctx->JavaEnv->DeleteLocalRef(jout_raw_name);
        mp_ctx->JavaEnv->DeleteLocalRef(jraw_name);

        // orig version
        jstring jstr;
        jstr = mp_ctx->JavaEnv->NewStringUTF((const char*)name);
        mp_ctx->JavaEnv->CallVoidMethod(mp_ctx->JavaMplayer, mp_ctx->jupdate_main_string, jstr);
        mp_ctx->JavaEnv->DeleteLocalRef(jstr);

    }

    void mp_main_init(uint8_t *src[])
    {
        qlog("here");
        mp_ctx->vm_attach_thread();
        mp_ctx->temp = mp_ctx->JavaEnv->NewDirectByteBuffer(src[0], mp_ctx->w1 * mp_ctx->h1 * 2);
        mp_ctx->video = (jobject)mp_ctx->JavaEnv->NewGlobalRef(mp_ctx->temp);
        mp_ctx->JavaEnv->CallVoidMethod(mp_ctx->JavaRenderer, mp_ctx->jupdate, mp_ctx->video);

        // temp=(*JavaEnv)->NewDirectByteBuffer(JavaEnv,src[0],w1*h1*2);
        // video=(jobject)(*JavaEnv)->NewGlobalRef(JavaEnv,temp);
        // (*JavaEnv)->CallVoidMethod( JavaEnv, JavaRenderer, jupdate,video);
    }

    void main_flip()
    {
        mp_ctx->vm_attach_thread();

        qlog("here");
        if (mp_ctx->m_play_btime == 0) {
            mp_ctx->m_play_btime = time(NULL);
        }
        mp_ctx->m_flip_count ++;

        // avg flip calc
        {
            int eclpase_time;

            mp_ctx->m_play_etime = time(NULL);
            eclpase_time = mp_ctx->m_play_etime - mp_ctx->m_play_btime;
            qlog("here: btime=%ld, etime=%ld, eclapse=%ld, flip_count=%d, avgflip=%d",
                 mp_ctx->m_play_btime, mp_ctx->m_play_etime, 
                 eclpase_time, mp_ctx->m_flip_count, 
                 eclpase_time == 0 ? 0 : (mp_ctx->m_flip_count/eclpase_time)
                 );
        }

        // real work
        // (*jniVM)->AttachCurrentThread(jniVM, &JavaEnv, NULL);
        // (*JavaEnv)->CallVoidMethod( JavaEnv, JavaRenderer, jupdate1);

        // 和这个没有关系，也就是应该不是View绘图慢引起的。
        mp_ctx->JavaEnv->CallVoidMethod(mp_ctx->JavaRenderer, mp_ctx->jupdate1);

    }

    void main_stop(int code)
    {
        mp_ctx->vm_attach_thread();

        int eclpase_time;
        qlog("here: %d", code);

        mp_ctx->m_play_etime = time(NULL);
        eclpase_time = mp_ctx->m_play_etime - mp_ctx->m_play_btime;
        qlog("here: %d, btime=%ld, etime=%ld, eclapse=%ld, flip_count=%d, avgflip=%d",
             code,
             mp_ctx->m_play_btime, mp_ctx->m_play_etime, 
             eclpase_time, mp_ctx->m_flip_count, 
             eclpase_time == 0 ? 0 : (mp_ctx->m_flip_count/eclpase_time)
             );


        mp_ctx->m_play_btime = 0;
        mp_ctx->m_play_etime = 0;
        mp_ctx->m_flip_count = 0;

        // real work
        // (*jniVM)->AttachCurrentThread(jniVM, &JavaEnv, NULL);
        // (*JavaEnv)->CallVoidMethod( JavaEnv, JavaMplayer, jstop,code);

        mp_ctx->JavaEnv->CallVoidMethod(mp_ctx->JavaMplayer, mp_ctx->jstop, code);
        
        qlog("here done");
        
    }

    void main_ao_init()
    {
        qlog("here");
        // (*jniVM)->AttachCurrentThread(jniVM, &JavaEnv, NULL);
        // (*JavaEnv)->CallVoidMethod( JavaEnv, JavaMplayer, jnativeAudio);
        mp_ctx->jniVM->AttachCurrentThread(&mp_ctx->JavaEnv, NULL);
        mp_ctx->JavaEnv->CallVoidMethod(mp_ctx->JavaMplayer, mp_ctx->jnativeAudio);
    }

    // rename to play_progress_update
    void update(int percent, int current, int total, int pause)
    {
        qlog("here");

        mp_ctx->vm_attach_thread();
        mp_ctx->JavaEnv->CallVoidMethod(mp_ctx->JavaMplayer, mp_ctx->jupdate_main,
                                        percent, current, total, pause);
        // (*jniVM)->AttachCurrentThread(jniVM, &JavaEnv, NULL);
        // (*JavaEnv)->CallVoidMethod( JavaEnv, JavaMplayer, jupdate_main,percent,current,total,pause);
    }

    int nativeConfig(int w, int h)
    {
        qlog("here");

        mp_ctx->w1 = w;
        mp_ctx->h1 = h;
        mp_ctx->i = 0;
        
        mp_ctx->vm_attach_thread();

        qlog("w=%d, h=%d", w, h);
        
        return(mp_ctx->JavaEnv->CallIntMethod(mp_ctx->JavaRenderer, mp_ctx->jnativeConfig, w, h));

        // w1=w;
        // h1=h;
        // i=0;
        // (*jniVM)->AttachCurrentThread(jniVM, &JavaEnv, NULL);
        // return((*JavaEnv)->CallIntMethod( JavaEnv, JavaRenderer, jnativeConfig,w,h ));

        return 0;
    }

    void nativeDeconfig()
    {
        qlog("here");

        mp_ctx->vm_attach_thread();
        mp_ctx->JavaEnv->DeleteGlobalRef(mp_ctx->video);
        mp_ctx->JavaEnv->CallVoidMethod(mp_ctx->JavaRenderer, mp_ctx->jnativeDeconfig);

        // (*jniVM)->AttachCurrentThread(jniVM, &JavaEnv, NULL);
        // (*JavaEnv)->DeleteGlobalRef(JavaEnv,video);
        // (*JavaEnv)->CallVoidMethod (JavaEnv, JavaRenderer, jnativeDeconfig);
    }

    extern int main(int argc, const char*argv[]);
    static void * mplayer_main_process(void *args) {
        // jniVM->AttachCurrentThread(jniVM, &JavaEnv, NULL);
        const char *argv[] = { "mplayer","-idle","-slave", "-v",
                               "-osdlevel","0","-vf",
                               "format=bgr16","-noaspect"
                               , "-nocorrect-pts",  "-quiet"   // "log=3"        // ,"-quiet"
                               , "-identify"
                               // , "-autosync", "30" 
                               , "-framedrop"
                                , "-fps", "23"
                               // , "-zoom", "-xy", "600"
                               // , "-vfm", "ffmpeg"
                               // , "-lavdopts", "lowres=1:fast:skiploopfilter=all"
        };
        int argc = sizeof(argv)/sizeof(argv[0]); // 12;
    
        __android_log_print(ANDROID_LOG_ERROR, "MPlayer", "Starting mplayer_main...");
        mp_ctx->set_mplayer_running(false);
        main(argc, argv);
        mp_ctx->set_mplayer_running(true);
        __android_log_print(ANDROID_LOG_ERROR, "MPlayer", "%s done", __FUNCTION__);
    }

}

extern "C" 
{
    #include "input/input.h"
    // mid layer
    void run_command_android(char *command)
    {
        qlog("attempt run cmd: %s", command);
        mp_cmd_t *cmd = mp_input_parse_cmd(command);
        if (cmd == NULL) {
            qlog("Invalid command: %s", command);
        }
        mp_input_queue_cmd(mp_input_parse_cmd(command));
    }

}

////////// jni bridge
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved)
{
    __android_log_print(ANDROID_LOG_ERROR, "MPlayer", "onload called");
    // jniVM = vm;
	// (*jniVM)->AttachCurrentThread(jniVM, &JavaEnv, NULL);
	// pthread_create(&native_thread,NULL,mainp,NULL);

    if (mp_ctx != NULL) {
        qlog("Why ctx not null: %p", mp_ctx);
    }
    mp_ctx = new MPlayerContext();

    mp_ctx->jniVM = vm;
    mp_ctx->jniVM->AttachCurrentThread(&mp_ctx->JavaEnv, NULL);

    pthread_create(&mp_ctx->native_thread, NULL, mplayer_main_process, NULL);

    return JNI_VERSION_1_2;
}

JNIEXPORT void JNICALL JNI_OnUnload(JavaVM *vm, void *reserved)
{
    // __android_log_print(ANDROID_LOG_ERROR, "MPlayer", "onunload called");
    // jniVM = vm;
    // run_command_android("quit");
    // pthread_exit(&native_thread);

    qlog("onunload called, %p=?%p =  %d", mp_ctx->jniVM, vm, mp_ctx->jniVM == vm);
    run_command_android("quit");
    pthread_exit(&mp_ctx->native_thread);

    delete mp_ctx;
    mp_ctx = NULL;
}

extern "C" {

    // simplify JNI FUNCTION definition
#define DEF_JNI_FUNC(rtype, func) JNIEXPORT rtype JNICALL Java_com_vnd_mplayer_MPlayer_##func
#define JNI_PARAMS    JNIEnv *env, jobject obj 

////////// called from java 
    DEF_JNI_FUNC(jint, startupPlayer)(JNI_PARAMS, jstring jparams)
    {
        qlog("startuping player.");
        mp_ctx->vm_attach_thread();

        char *args[100];
        const char *params;
        const char *s, *p, *p1;
        
        params = mp_ctx->JavaEnv->GetStringUTFChars(jparams, NULL);
        s = params;
        

        pthread_create(&mp_ctx->native_thread, NULL, mplayer_main_process, NULL);
    }

    DEF_JNI_FUNC(jint, shutdownPlayer)(JNI_PARAMS)
    {
        qlog("shutdowning player.");
        run_command_android("quit");
        pthread_exit(&mp_ctx->native_thread);
    }

    JNIEXPORT void JNICALL Java_com_vnd_mplayer_PlasmaView_init(JNIEnv *env, jobject  obj,jint delay)
    {
        // (*jniVM)->AttachCurrentThread(jniVM, &JavaEnv, NULL);
        //  JavaEnv = env;
        //  JavaRenderer = obj;
        //  JavaRendererClass = (*JavaEnv)->GetObjectClass(JavaEnv, obj);
        //  jnativeConfig = (*JavaEnv)->GetMethodID(JavaEnv, JavaRendererClass, "jnativeConfig", "(II)I");
        //  jnativeDeconfig = (*JavaEnv)->GetMethodID(JavaEnv, JavaRendererClass, "jnativeDeconfig", "()V");
        //  jupdate = (*JavaEnv)->GetMethodID(JavaEnv, JavaRendererClass, "update", "(Ljava/nio/ByteBuffer;)V");
        //  jupdate1 = (*JavaEnv)->GetMethodID(JavaEnv, JavaRendererClass, "update1", "()V");

        qlog("plasma view init called.");

        mp_ctx->jniVM->AttachCurrentThread(&mp_ctx->JavaEnv, NULL);
        mp_ctx->JavaEnv = env;
        mp_ctx->JavaRenderer = obj;
        mp_ctx->JavaRendererClass = mp_ctx->JavaEnv->GetObjectClass(obj);
        mp_ctx->jnativeConfig = mp_ctx->JavaEnv->GetMethodID(mp_ctx->JavaRendererClass, "jnativeConfig", "(II)I");
        mp_ctx->jnativeDeconfig = mp_ctx->JavaEnv->GetMethodID(mp_ctx->JavaRendererClass, "jnativeDeconfig", "()V");
        mp_ctx->jupdate = mp_ctx->JavaEnv->GetMethodID(mp_ctx->JavaRendererClass, "update", "(Ljava/nio/ByteBuffer;)V");
        mp_ctx->jupdate1 = mp_ctx->JavaEnv->GetMethodID(mp_ctx->JavaRendererClass, "update1", "()V");
    }



    JNIEXPORT void JNICALL Java_com_vnd_mplayer_MPlayer_nativeExit(JNIEnv *env, jobject  obj)
    {
        //__android_log_print(ANDROID_LOG_ERROR,"MPlayer","stop called");
        qlog("native exit called.");
        run_command_android("stop");
    }

    JNIEXPORT void JNICALL Java_com_vnd_mplayer_MPlayer_play(JNIEnv *env, jobject  obj,jstring fname,int cpuflags)
    {
        qlog("player play called.");

        // (*jniVM)->AttachCurrentThread(jniVM, &JavaEnv, NULL);
        // JavaMplayer = obj;
        // JavaMplayerClass = (*JavaEnv)->GetObjectClass(JavaEnv, obj);
        // jnativeAudio = (*JavaEnv)->GetMethodID(JavaEnv, JavaMplayerClass, "jnativeAudio", "()V");
        // jstop = (*JavaEnv)->GetMethodID(JavaEnv, JavaMplayerClass, "stop", "(I)V");
        // jupdate_main = (*JavaEnv)->GetMethodID(JavaEnv, JavaMplayerClass, "update_main", "(IIII)V");
        // jupdate_main_string = (*JavaEnv)->GetMethodID(JavaEnv, JavaMplayerClass, "update_main_string", "(Ljava/lang/String;)V");
        // if (cpuflags==1)
        //     arm_vfp=1;
        // if (cpuflags==2)
        //     arm_neon=1;
        // if (cpuflags==3){
        //     arm_vfp=1;arm_neon=1;}
        // str = (*JavaEnv)->GetStringUTFChars(JavaEnv, fname, NULL);
        // char *temp;
        // temp=malloc(255);
        // if ((strstr(str,".m3u")!=NULL) || (strstr(str,".pls")!=NULL))
        //     strcpy(temp,"loadlist \"");
        // else
        //     strcpy(temp,"loadfile \"");
        // strcat(temp,str);
        // strcat(temp,"\"");
        // run_command_android(temp);
        // free(temp);

        mp_ctx->jniVM->AttachCurrentThread(&mp_ctx->JavaEnv, NULL);
        mp_ctx->JavaMplayer = obj;
        mp_ctx->JavaMplayerClass = mp_ctx->JavaEnv->GetObjectClass(obj);
        mp_ctx->jnativeAudio = mp_ctx->JavaEnv->GetMethodID(mp_ctx->JavaMplayerClass, "jnativeAudio", "()V");
        mp_ctx->jstop = mp_ctx->JavaEnv->GetMethodID(mp_ctx->JavaMplayerClass, "stop", "(I)V");
        mp_ctx->jupdate_main = mp_ctx->JavaEnv->GetMethodID(mp_ctx->JavaMplayerClass, "update_main", "(IIII)V");
        mp_ctx->jupdate_main_string = mp_ctx->JavaEnv->GetMethodID(mp_ctx->JavaMplayerClass, "update_main_string", "(Ljava/lang/String;)V");
        if (cpuflags == 1) {
            mp_ctx->arm_vfp = 1;
        }
        if (cpuflags == 2) {
            mp_ctx->arm_neon = 1;
        }
        if (cpuflags == 3) {
            mp_ctx->arm_vfp = 1;
            mp_ctx->arm_neon = 1;
        }

        mp_ctx->str = (const jbyte*)mp_ctx->JavaEnv->GetStringUTFChars(fname, NULL);
        char temp[255];
        // temp = malloc(255);
        if ((strstr((char *)mp_ctx->str, ".m3u") != NULL) || (strstr((char*)mp_ctx->str, ".pls") != NULL))
            strcpy(temp, "loadlist \"");
        else
            strcpy(temp, "loadfile \"");
        strcat(temp, (char*)mp_ctx->str);
        strcat(temp, "\"");
        run_command_android(temp);
        // free(temp);
    }

    extern int isPaused(); // in mplayer.c last line
    JNIEXPORT void JNICALL Java_com_vnd_mplayer_MPlayer_nativeCommand(JNIEnv *env, jobject obj, jint command, jint x, jint y)
    {
        char *mpcmd = NULL;
        switch (command){
        case 1:	//Pause
            if (x==0 && isPaused()==1)
                run_command_android("pause");
            if (x==1 && isPaused()==0)
                run_command_android("pause");
            if (x==2)
                run_command_android("pause");
            break;
        case 2:  //Seek
            mpcmd = (char *)malloc(20);
            sprintf(mpcmd, "seek %d 1", x);
            run_command_android(mpcmd);
            free((void *)mpcmd);
            break;
        case 3:
            if (x==1)
                run_command_android("osd 3");
            else
                run_command_android("osd 0");
            break;
        case 4:
            if (x==1)
                run_command_android("pt_step 1 1");
            else
                run_command_android("pt_step -1 1");
            break;
		
        }
    }

    DEF_JNI_FUNC(jint, nativeRawCommand)(JNI_PARAMS, jstring jcommand)
    {
        if (env != mp_ctx->JavaEnv) {
            qlog("Why two env not equal: %p=?%p", env, mp_ctx->JavaEnv);
        }
        mp_ctx->vm_attach_thread();

        const char *command;
        
        command = env->GetStringUTFChars(jcommand, NULL);
        run_command_android((char*)command);
        env->ReleaseStringUTFChars(jcommand, command);
    }
    
    DEF_JNI_FUNC(void, hhhhhhhhgggggggggggggggggggggg)()
    {
    }

    JNIEXPORT jint JNICALL Java_com_vnd_mplayer_MPlayer_ispaused(JNIEnv *env, jobject  obj)
    {
        return isPaused();
        return 0;
    }

    JNIEXPORT int JNICALL Java_com_vnd_mplayer_MPlayer_nativeTouch(JNIEnv *env, jobject thiz, jfloat x, jfloat y)
    {
        if (x==0)
            run_command_android("seek +1");
        else
            run_command_android("seek -1");

    }


    JNIEXPORT int JNICALL Java_com_vnd_mplayer_MPlayer_quit(JNIEnv *env, jclass cls)
    {
        run_command_android("quit");
    }

    JNIEXPORT int JNICALL Java_com_vnd_mplayer_MPlayer_isalbum(JNIEnv *env, jobject obj, jstring fname, jstring outfile)
    {
        mp_ctx->vm_attach_thread();
        

        // (*jniVM)->AttachCurrentThread(jniVM, &JavaEnv, NULL);
        // const char *str,*fileout;
        // str = (*env)->GetStringUTFChars(env,fname, NULL);
        // fileout = (*env)->GetStringUTFChars(env,outfile, NULL);
        // if (isalbum(str,fileout)){
        //     (*env)->ReleaseStringUTFChars(env, fname, str);
        //     (*env)->ReleaseStringUTFChars(env, outfile, fileout);
        //     return 1;
        // }
        // (*env)->ReleaseStringUTFChars(env, fname, str);
        // (*env)->ReleaseStringUTFChars(env, outfile, fileout);
        // return 0;
    }


}

/*
static JNIEnv* JavaEnv = NULL;
static jclass JavaRendererClass = NULL;
static jclass JavaMplayerClass = NULL;
static jobject JavaRenderer = NULL;
static jobject JavaMplayer = NULL;
static jmethodID jnativeConfig = NULL;
static jmethodID jnativeAudio = NULL;
static jmethodID jnativeDeconfig = NULL;
static jmethodID jupdate = NULL;
static jmethodID jupdate1 = NULL;
static jmethodID jupdate_main = NULL;
static jmethodID jupdate_main_string = NULL;
static jmethodID jstop = NULL;
static jobject video;
pthread_t native_thread;
const jbyte *str;
char* command = NULL;
static JavaVM *jniVM=NULL;
int arm_vfp=0;
int arm_neon=0;
static int w1,h1;
jobject temp;
int i,x;

void update_main_string(uint8_t *name){
jstring jstr;
(*jniVM)->AttachCurrentThread(jniVM, &JavaEnv, NULL);
jstr=(*JavaEnv)->NewStringUTF(JavaEnv,name);
(*JavaEnv)->CallVoidMethod( JavaEnv,JavaMplayer, jupdate_main_string,jstr);
(*JavaEnv)->DeleteLocalRef(JavaEnv,jstr);
}

void main_init(uint8_t *src[]){
	(*jniVM)->AttachCurrentThread(jniVM, &JavaEnv, NULL);
        temp=(*JavaEnv)->NewDirectByteBuffer(JavaEnv,src[0],w1*h1*2);
        video=(jobject)(*JavaEnv)->NewGlobalRef(JavaEnv,temp);
        (*JavaEnv)->CallVoidMethod( JavaEnv, JavaRenderer, jupdate,video);
}

void main_flip(){
	(*jniVM)->AttachCurrentThread(jniVM, &JavaEnv, NULL);
	(*JavaEnv)->CallVoidMethod( JavaEnv, JavaRenderer, jupdate1);
}


void main_stop(int code){
	(*jniVM)->AttachCurrentThread(jniVM, &JavaEnv, NULL);
	(*JavaEnv)->CallVoidMethod( JavaEnv, JavaMplayer, jstop,code);
}

void main_ao_init(){
(*jniVM)->AttachCurrentThread(jniVM, &JavaEnv, NULL);
(*JavaEnv)->CallVoidMethod( JavaEnv, JavaMplayer, jnativeAudio);
}

void update(int percent,int current,int total,int pause){
(*jniVM)->AttachCurrentThread(jniVM, &JavaEnv, NULL);
(*JavaEnv)->CallVoidMethod( JavaEnv, JavaMplayer, jupdate_main,percent,current,total,pause);
}

JNIEXPORT void JNICALL Java_com_vnd_mplayer_PlasmaView_init(JNIEnv *env, jobject  obj,jint delay)
{
	(*jniVM)->AttachCurrentThread(jniVM, &JavaEnv, NULL);
	 JavaEnv = env;
 	 JavaRenderer = obj;
	 JavaRendererClass = (*JavaEnv)->GetObjectClass(JavaEnv, obj);
	 jnativeConfig = (*JavaEnv)->GetMethodID(JavaEnv, JavaRendererClass, "jnativeConfig", "(II)I");
	 jnativeDeconfig = (*JavaEnv)->GetMethodID(JavaEnv, JavaRendererClass, "jnativeDeconfig", "()V");
	 jupdate = (*JavaEnv)->GetMethodID(JavaEnv, JavaRendererClass, "update", "(Ljava/nio/ByteBuffer;)V");
	 jupdate1 = (*JavaEnv)->GetMethodID(JavaEnv, JavaRendererClass, "update1", "()V");
}


JNIEXPORT void JNICALL Java_com_vnd_mplayer_MPlayer_nativeExit(JNIEnv *env, jobject  obj)
{
	//__android_log_print(ANDROID_LOG_ERROR,"MPlayer","stop called");
	run_command_android("stop");
}

JNIEXPORT void JNICALL Java_com_vnd_mplayer_MPlayer_play(JNIEnv *env, jobject  obj,jstring fname,int cpuflags){
(*jniVM)->AttachCurrentThread(jniVM, &JavaEnv, NULL);
 JavaMplayer = obj;
 JavaMplayerClass = (*JavaEnv)->GetObjectClass(JavaEnv, obj);
 jnativeAudio = (*JavaEnv)->GetMethodID(JavaEnv, JavaMplayerClass, "jnativeAudio", "()V");
 jstop = (*JavaEnv)->GetMethodID(JavaEnv, JavaMplayerClass, "stop", "(I)V");
 jupdate_main = (*JavaEnv)->GetMethodID(JavaEnv, JavaMplayerClass, "update_main", "(IIII)V");
 jupdate_main_string = (*JavaEnv)->GetMethodID(JavaEnv, JavaMplayerClass, "update_main_string", "(Ljava/lang/String;)V");
if (cpuflags==1)
arm_vfp=1;
if (cpuflags==2)
arm_neon=1;
if (cpuflags==3){
arm_vfp=1;arm_neon=1;}
str = (*JavaEnv)->GetStringUTFChars(JavaEnv, fname, NULL);
char *temp;
temp=malloc(255);
if ((strstr(str,".m3u")!=NULL) || (strstr(str,".pls")!=NULL))
strcpy(temp,"loadlist \"");
else
strcpy(temp,"loadfile \"");
strcat(temp,str);
strcat(temp,"\"");
run_command_android(temp);
free(temp);
}


JNIEXPORT void JNICALL Java_com_vnd_mplayer_MPlayer_nativeCommand(JNIEnv *env, jobject  obj,jint command,jint x,jint y)
{

switch (command){
	case 1:	//Pause
		if (x==0 && isPaused()==1)
		run_command_android("pause");
		if (x==1 && isPaused()==0)
		run_command_android("pause");
		if (x==2)
		run_command_android("pause");
		break;
	case 2:  //Seek
		command=(void *)malloc(20);
		sprintf((void *)command,"seek %d 1",x);
		run_command_android(command);
		free((void *)command);
		break;
	case 3:
		if (x==1)
		 run_command_android("osd 3");
		else
		 run_command_android("osd 0");
		break;
	case 4:
		if (x==1)
		 run_command_android("pt_step 1 1");
		else
		run_command_android("pt_step -1 1");
		break;
		
}
}

JNIEXPORT jint JNICALL Java_com_vnd_mplayer_MPlayer_ispaused(JNIEnv *env, jobject  obj){
(*jniVM)->AttachCurrentThread(jniVM, &JavaEnv, NULL);
return isPaused();
}

int nativeConfig(int w,int h){
//mp_msg(MSGT_CPLAYER,MSGL_ERR,"nativeConfig called %d %d",w,h);
w1=w;
h1=h;
i=0;
(*jniVM)->AttachCurrentThread(jniVM, &JavaEnv, NULL);
return((*JavaEnv)->CallIntMethod( JavaEnv, JavaRenderer, jnativeConfig,w,h ));
}


void nativeDeconfig(){
(*jniVM)->AttachCurrentThread(jniVM, &JavaEnv, NULL);
(*JavaEnv)->DeleteGlobalRef(JavaEnv,video);
(*JavaEnv)->CallVoidMethod (JavaEnv, JavaRenderer, jnativeDeconfig);
}

JNIEXPORT int JNICALL Java_com_vnd_mplayer_MPlayer_nativeTouch(JNIEnv *env, jobject thiz,jfloat x,jfloat y) {

if (x==0)
run_command_android("seek +1");
else
run_command_android("seek -1");

}

//JNIEXPORT int JNICALL Java_com_vnd_mplayer_MPlayer_main(JNIEnv *env, jclass cls) {
void mainp(void){
(*jniVM)->AttachCurrentThread(jniVM, &JavaEnv, NULL);
int argc=10;char *argv[] ={ "mplayer","-idle","-slave","-osdlevel","0","-vf","format=bgr16","-noaspect","-nocorrect-pts","-quiet"};
main(argc,argv);
}


JNIEXPORT int JNICALL Java_com_vnd_mplayer_MPlayer_quit(JNIEnv *env, jclass cls) {
run_command_android("quit");
}


JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved)
{
        jniVM = vm;
	(*jniVM)->AttachCurrentThread(jniVM, &JavaEnv, NULL);
	pthread_create(&native_thread,NULL,mainp,NULL);
        return JNI_VERSION_1_2;
};

JNIEXPORT void JNICALL JNI_OnUnload(JavaVM *vm, void *reserved)
{
       	 jniVM = vm;
	 run_command_android("quit");
	 pthread_exit(&native_thread);
};

void run_command_android(char *command){
mp_input_queue_cmd(mp_input_parse_cmd(command));
}

JNIEXPORT int JNICALL Java_com_vnd_mplayer_MPlayer_isalbum(JNIEnv *env, jobject  obj,jstring fname,jstring outfile){
(*jniVM)->AttachCurrentThread(jniVM, &JavaEnv, NULL);
const char *str,*fileout;
str = (*env)->GetStringUTFChars(env,fname, NULL);
fileout = (*env)->GetStringUTFChars(env,outfile, NULL);
if (isalbum(str,fileout)){
(*env)->ReleaseStringUTFChars(env, fname, str);
(*env)->ReleaseStringUTFChars(env, outfile, fileout);
return 1;
}
(*env)->ReleaseStringUTFChars(env, fname, str);
(*env)->ReleaseStringUTFChars(env, outfile, fileout);
return 0;
}
int have_neon(){
return arm_neon;
}

*/
