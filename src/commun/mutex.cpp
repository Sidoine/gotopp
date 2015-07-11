#ifdef _WIN32
#include <windows.h>
#else
#include <pthread.h>
#endif
#include <gotopp/mutex.h>

using namespace GotoPP;

Mutex::Mutex()
{
#ifdef _WIN32
	*(HANDLE*)_data=CreateMutex(NULL,FALSE,NULL);
#else
	pthread_mutex_init((pthread_mutex_t*)_data,NULL);
#endif
}

Mutex::~Mutex()
{
#ifdef _WIN32
	CloseHandle(*(HANDLE*)_data);
#else
	pthread_mutex_destroy((pthread_mutex_t*)_data);
#endif
}

void Mutex::Liberer()
{
#ifdef _WIN32
	ReleaseMutex(*(HANDLE*)_data);
#else
	pthread_mutex_unlock((pthread_mutex_t*)_data);
#endif
}

void Mutex::Prendre()
{
#ifdef _WIN32
	WaitForSingleObject(*(HANDLE*)_data,INFINITE);
#else
	pthread_mutex_lock((pthread_mutex_t*)_data);
#endif
}

/*
typedef void* MUTEX;
#define PRENDREMUTEX WaitForSingleObject(Mutex,INFINITE)
#define LIBERERMUTEX ReleaseMutex(Mutex)
#define PRENDREINTMUTEX WaitForSingleObject(Int->Mutex,INFINITE)
#define LIBERERINTMUTEX ReleaseMutex(Int->Mutex)

typedef pthread_mutex_t MUTEX;
#define PRENDREMUTEX pthread_mutex_lock(&Mutex)
#define LIBERERMUTEX pthread_mutex_unlock(&Mutex)
#define PRENDREINTMUTEX pthread_mutex_lock(&Int->Mutex)
#define LIBERERINTMUTEX pthread_mutex_unlock(&Int->Mutex)
#endif*/
