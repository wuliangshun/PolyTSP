#include "thread.h"





void thread_proc(void* (*func)(void*), void* args)
{
	#ifdef _WIN32
		HANDLE handle[NUM_THREADS];
		for(int i=0; i<NUM_THREADS; ++i)
		{
			handle[i] = CreateThread(NULL, 0, *func, args, 0, NULL);	
		}
		for(int i=0; i<NUM_THREADS; ++i)
		{
			WaitForSingleObject(handle[i], INFINITE);
			CloseHandle(handle[i]);			
		}
	#else
		pthread_t tids[NUM_THREADS];
		for(int i=0; i< NUM_THREADS; ++i)
		{
			int ret = pthread_create(&tids[i], NULL, *func, args);
		}
		//pthread_exit(NULL);
		for(int i=0; i< NUM_THREADS; ++i)
			pthread_join(tids[i],NULL); 
		
		
	
	#endif
}



void thread_proc_ret(void* (*func)(void*), void* args, vector<void*>& p_vec)
{		
	pthread_t tids[NUM_THREADS];
	
	for(int i=0; i < NUM_THREADS; ++i)
	{
		int ret = pthread_create(&tids[i], NULL, *func, args);
	}	
	for(int i=0; i < NUM_THREADS; ++i)
	{
		void* p = new void*; // function return
		pthread_join(tids[i], &p);
		p_vec.push_back(p);
	}
}