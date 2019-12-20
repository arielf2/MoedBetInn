#include "ThreadFuncs.h"
#include "Header.h"

int CreateThreadsForGuests(guest *guests_array[]) {

}

HANDLE CreateThreadSimple(LPTHREAD_START_ROUTINE p_start_routine, LPVOID p_thread_parameters, LPDWORD p_thread_id)
{
	HANDLE thread_handle = NULL;

	if (NULL == p_start_routine)
	{
		printf("Error when creating a thread");
		printf("Received null pointer");
		exit(1);
	}

	if (NULL == p_thread_id)
	{
		printf("Error when creating a thread");
		printf("Received null pointer");
		exit(1);
	}

	thread_handle = CreateThread(
		NULL,                /*  default security attributes */
		0,                   /*  use default stack size */
		p_start_routine,     /*  thread function */
		p_thread_parameters, /*  argument to thread function */
		0,                   /*  use default creation flags */
		p_thread_id);        /*  returns the thread identifier */

	return thread_handle;
}


DWORD WINAPI GuestThread(LPVOID lpParam)
{
	guest *guest_params;
	int return_val = 0;

	/* Check if lpParam is NULL */
	if (NULL == lpParam)
	{
		return -1;
	}

	/*
	* Convert (void *) to parameters type.
	*/
	guest_params = (guest*)lpParam;

	return_val = guest_function(guest_params);

	return return_val;
}

int guest_function(guest* guest) {
/*	how a guest struct looks:

	char name[ROOM_GUEST_NAME_LEN];
	int money;

	char suitable_room[ROOM_GUEST_NAME_LEN];
	int num_of_nights;

 */
	HANDLE room_semaphore = OpenSemaphore(SYNCHRONIZE, FALSE, guest->suitable_room);
	HANDLE log_file_mutex = NULL;
	while (1) {
		int waitcode = WaitForSingleObject(room_semaphore, TIMEOUT);
		// check waitcodes
		if (waitcode == WAIT_OBJECT_0) {	/* guest can enter the room */
			log_file_mutex = OpenMutex(SYNCHRONIZE, FALSE, "logFileMutex");
			waitcode = WaitForSingleObject(log_file_mutex, INFINITE);
			if (waitcode == WAIT_OBJECT_0) {
				/* means no one is writing the file, the mutex is "open" for this thread to write */
				/* write to log file */
			}
			else { /* check waitcodes*/

			}


		}
		else {
			/* guest must wait - TIMEOUT */
		}
	}
	
	


	/* if waitcode is successful:
	
	
	if waitcode timeout:
	*/
}

