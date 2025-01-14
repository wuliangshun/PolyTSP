#include "log.h"

#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <cstring>
#include <sys/stat.h>
#include <unistd.h>

#ifdef WIN32
#include <windows.h>
#else
#include <pthread.h>
#endif

#ifndef _CLOGSTATIC
#define _CLOGSTATIC

const char CLOGLevelName[4][8] = { "debug", "info", "warning", "error" };
FILE *fp = NULL;
char now_file_name[11];
char *log_path;

#ifdef WIN32
CRITICAL_SECTION g_cs;
#else
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
#endif
#endif
void now_date_str(char *date_str)
{
	time_t nowtime;
	struct tm *timeinfo;
	time(&nowtime);
	timeinfo = localtime(&nowtime);
	sprintf(date_str, "%d_%d_%d",
		timeinfo->tm_year + 1900,
		timeinfo->tm_mon + 1,
		timeinfo->tm_mday);
}

void now_datetime_str(char *datetime_str)
{
	time_t nowtime;
	struct tm *timeinfo;
	time(&nowtime);
	timeinfo = localtime(&nowtime);
	sprintf(datetime_str, "%d_%d_%d %d:%d:%d",
		timeinfo->tm_year + 1900,
		timeinfo->tm_mon + 1,
		timeinfo->tm_mday,
		timeinfo->tm_hour,
		timeinfo->tm_min,
		timeinfo->tm_sec
		);
}

void lock()
{
#ifdef WIN32
	EnterCriticalSection(&g_cs);
#else
	pthread_mutex_lock(&mutex1);
#endif
}

void unlock()
{
#ifdef WIN32
	LeaveCriticalSection(&g_cs);
#else
	pthread_mutex_unlock(&mutex1);
#endif
}

long thread_id()
{
#ifdef WIN32
	return GetCurrentThreadId();
#else
	return pthread_self();
#endif
}

void open_log_file()
{
	char *filename = (char*)malloc(strlen(log_path) + strlen(now_file_name) + 1);
	sprintf(filename, "%s/%s.%s", log_path, now_file_name, "txt");
	fp = fopen(filename, "ab+");
	if (fp == NULL)
	{
		stderr_message(fp == NULL, "(clog) fopen error !");
		return;
	}
}

//mk dirs
void mkdir_p(char *dir)
{
	//copy
	char * dir_c = (char*)malloc(strlen(dir) + 1);
	memcpy(dir_c, dir, strlen(dir) + 1);
	dir = dir_c;

	char * temppath = (char*)malloc(strlen(dir) + 1);
	int tempindex = 0;
	while (*dir_c != '\0')
	{
		if (*dir_c == '\\')
			*dir_c = '/';
		if (*dir_c == '/')
		{
			tempindex = dir_c - dir;
			memcpy(temppath, dir, tempindex);
			temppath[tempindex] = '\0';
			#ifdef WIN32
				if (_access(temppath, 0) != 0)
					_mkdir(temppath);
			#else
				if (access(temppath, 0) != 0)
					mkdir(temppath,S_IRWXU | S_IRWXG | S_IRWXO);
			#endif			
		}
		dir_c++;
	}
	#ifdef WIN32
		if (_access(dir, 0) != 0)
			_mkdir(dir);
	#else
		if (access(dir, 0) != 0)
			mkdir(dir,S_IRWXU | S_IRWXG | S_IRWXO); 
	#endif
	free(dir);
	free(temppath);
}

void InitCLog(char *path)
{

#ifdef WIN32
	InitializeCriticalSection(&g_cs);
#endif
	if (path == NULL)
		stderr_message(path == NULL, "(clog) logpath is null !");
	now_date_str(now_file_name);
	int pathlength = strlen(path);
	log_path = (char*)malloc(pathlength*sizeof(char) + 1);
	strcpy(log_path, path);
	if (log_path[pathlength - 1] == '/')
		log_path[pathlength - 1] = '\0';
	mkdir_p(log_path);
	open_log_file();
}

void _CLog(int level, char *msg, int line, char *file)
{
	lock();
	if (level<0 || level>3)
	{
		stderr_message(level<0 || level>3, "(clog) level overflow!");
		return;
	}
	if (fp == NULL)
	{
		stderr_message(fp == NULL, "(clog) clog not init!");
		return;
	}
	char temp_now_file_name[11];
	now_date_str(temp_now_file_name);
	if (strcmp(temp_now_file_name, now_file_name) != 0)
	{
		strcpy(now_file_name, temp_now_file_name);
		fclose(fp);
		open_log_file();
		if (fp == NULL)
		{
			stderr_message(fp == NULL, "(clog) clog init error!");
			return;
		}
	}
	char* info = (char*)malloc(strlen(msg) + 80 + strlen(file));
	char datetimestr[21];
	now_datetime_str(datetimestr);
	sprintf(info, "%s thread:%d %s \r\n%s\r\nfile:%s line:%d\r\n",
		datetimestr,
		thread_id(),
		CLOGLevelName[level],
		msg,
		file,
		line);
	fwrite(info, strlen(info), 1, fp);
	fflush(fp);
	free(info);
	unlock();
}

void FreeeClog()
{
	lock();
	if (fp != NULL)
		fclose(fp);
	free(log_path);
	unlock();
}