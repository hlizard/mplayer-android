
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "version.h"
#include "codec-cfg.h"

#include "mplayer-info.h"

extern codecs_t *video_codecs;
extern codecs_t *audio_codecs;
extern int nr_vcodecs;
extern int nr_acodecs;

#include <math.h>


// #define log2f(x) (logf(x)*1.4426950408889634f)
double log2f(double x) ;
inline double log2f(double x) 
{
    return logf(x)*1.4426950408889634f;
}


char * get_codecs_list(int audioflag) {
    int i;
    codecs_t *c;
    char wbuf[256];
    char *codecs_list_str = calloc(1, 32 * 1024);
    
    memset(codecs_list_str, 0, 32  * 1024);
    memset(wbuf, 0, sizeof(wbuf));
    if (audioflag) {
        i = nr_acodecs;
        c = audio_codecs;
        // mp_msg(MSGT_CODECCFG,MSGL_INFO,"ac:     afm:      status:   info:  [lib/dll]\n");
        snprintf(wbuf, sizeof(wbuf)-1, "ac:     afm:      status:   info:  [lib/dll]\n");
    } else {
        i = nr_vcodecs;
        c = video_codecs;
        // mp_msg(MSGT_CODECCFG,MSGL_INFO,"vc:     vfm:      status:   info:  [lib/dll]\n");
        snprintf(wbuf, sizeof(wbuf)-1, "vc:     vfm:      status:   info:  [lib/dll]\n");
    }
    strcat(codecs_list_str, wbuf);

    if(!i) return codecs_list_str;
    for (/* NOTHING */; i--; c++) {
        char* s="unknown ";
        switch(c->status){
        case CODECS_STATUS_WORKING:     s="working ";break;
        case CODECS_STATUS_PROBLEMS:    s="problems";break;
        case CODECS_STATUS_NOT_WORKING: s="crashing";break;
        case CODECS_STATUS_UNTESTED:    s="untested";break;
        }
        memset(wbuf, 0, sizeof(wbuf));
        if(c->dll) {
            // mp_msg(MSGT_CODECCFG,MSGL_INFO,"%-11s %-9s %s  %s  [%s]\n",c->name,c->drv,s,c->info,c->dll);
            snprintf(wbuf, sizeof(wbuf)-1, "%-11s %-9s %s  %s  [%s]\n",c->name,c->drv,s,c->info,c->dll);
        } else {
            // mp_msg(MSGT_CODECCFG,MSGL_INFO,"%-11s %-9s %s  %s\n",c->name,c->drv,s,c->info);
            snprintf(wbuf, sizeof(wbuf)-1, "%-11s %-9s %s  %s\n",c->name,c->drv,s,c->info);
        }
        strcat(codecs_list_str, wbuf);
    }
    return codecs_list_str;
}


char *get_mplayer_version ()
{
    char *ver_str = NULL;

    ver_str = strdup(ver_str);

    return ver_str;
}
