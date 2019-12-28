#pragma once

#include <windows.h>
//int CreateThreadsForGuests(guest *guests_array[]);
#define TIMEOUT 8000


HANDLE CreateThreadSimple(LPTHREAD_START_ROUTINE p_start_routine, LPVOID p_thread_parameters, LPDWORD p_thread_id);

DWORD WINAPI GuestThread(LPVOID lpParam);

//int guest_function(thread_param_struct* thread_param);
