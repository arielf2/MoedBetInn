#pragma once
#include "Header.h"
#include <windows.h>

/* this will include semaphore and mutex functions.. */
void CreateRoomSemaphores(HANDLE semaphore_array[], room *rooms_array[], int num_of_rooms);
