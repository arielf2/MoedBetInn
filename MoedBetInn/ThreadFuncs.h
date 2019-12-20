#pragma once

#include <windows.h>
//int CreateThreadsForGuests(guest *guests_array[]);
#define TIMEOUT 5000

HANDLE CreateThreadSimple(LPTHREAD_START_ROUTINE p_start_routine, LPVOID p_thread_parameters, LPDWORD p_thread_id);

DWORD WINAPI GuestThread(LPVOID lpParam);
