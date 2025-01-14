#include <stdlib.h>

#pragma once
#ifndef _CLOG
#define _CLOG

#define CLOG_DEBUG		 0
#define CLOG_INFO		 1
#define CLOG_WARNING	 2
#define CLOG_ERROR		 3

#define stderr_message(exp, message) { fprintf(stderr,"-------------ERROR-----------------\n"); fprintf(stderr,"Failed:%s\nMessage:%s\nLine:%d\nFile:%s\n",#exp,message,__LINE__,__FILE__);fprintf(stderr,"-------------ERROR-----------------\n"); /*exit(EXIT_FAILURE);*/}


//Clog(int level, char *msg)
#define CLog(level,msg){_CLog(level,msg,__LINE__,__FILE__);}
//CLogDebug(char *msg)
#define CLogDebug(msg){CLog(CLOG_DEBUG,msg);}
//CLogError(char *msg)
#define CLogError(msg){CLog(CLOG_ERROR,msg);}
//CLogInfo(char *msg)
#define CLogInfo(msg){CLog(CLOG_INFO,msg);}
//CLogWarning(char *msg)
#define CLogWarning(msg){CLog(CLOG_WARNING,msg);}
#endif

#ifdef __cplusplus
extern "C" {
#endif

	//init clog ! log_path is logfile path!
	void InitCLog(char *log_path);

	//this fun is logging!
	void _CLog(int level, char *msg,int line,char *file);

	//freee clog!
	void FreeeClog();


#ifdef __cplusplus
}
#endif
