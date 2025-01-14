#ifndef __THREAD_H__
#define __THREAD_H__

#define NUM_THREADS 200

#ifdef _WIN32
	#include <windows.h>
#else
	#include <pthread.h>
#endif
#include <vector>
using namespace std;

void thread_proc(void* (*func)(void*), void* args);

void thread_proc_ret(void* (*func)(void*), void* args, vector<void*>& p_vec);
#endif
