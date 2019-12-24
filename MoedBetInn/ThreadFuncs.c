#include "ThreadFuncs.h"
#include "Header.h"

//int CreateThreadsForGuests(guest *guests_array[]) {
//
//}

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
	//guest *guest_params;
	thread_param_struct *thread_param;
	int return_val = 0;

	/* Check if lpParam is NULL */
	if (NULL == lpParam)
	{
		return -1;
	}

	/*
	* Convert (void *) to parameters type.
	*/
	thread_param = (thread_param_struct*)lpParam;

	return_val = guest_function(thread_param);

	return return_val;
}

int guest_function(thread_param_struct* thread_param) {
/*	how a guest struct looks:

	char name[ROOM_GUEST_NAME_LEN];
	int money;

	char suitable_room[ROOM_GUEST_NAME_LEN];
	int num_of_nights;

 */
	HANDLE room_semaphore = OpenSemaphore(SYNCHRONIZE, FALSE, thread_param->guest->suitable_room);
	HANDLE log_file_mutex = NULL;
	HANDLE count_mutex = NULL;
	int start_day = 0;
	int room_wait_code = 0;
	int count_wait_code = 0;
	int file_wait_code = 0;
	int roomlog_file_error = 0;
	FILE *roomlog_fp;
	int error_code = 0;
	while (1) {

		room_wait_code = WaitForSingleObject(room_semaphore, TIMEOUT);
		// check waitcodes
		if (room_wait_code == WAIT_OBJECT_0) {	/* guest can enter the room */
			start_day = *(thread_param->day);
			log_file_mutex = OpenMutex(SYNCHRONIZE, FALSE, "logFileMutex");
			count_mutex = OpenMutex(SYNCHRONIZE, FALSE, "countMutex");

			file_wait_code = WaitForSingleObject(log_file_mutex, INFINITE);
			if (file_wait_code == WAIT_OBJECT_0) {
				/* means no one is writing the file, the mutex is "open" for this thread to write */
				/* write to log file */
				WriteToRoomLog(thread_param->guest->name, thread_param->guest->suitable_room, &roomlog_fp, start_day);
				error_code = ReleaseMutex(log_file_mutex);
			
				if (error_code == 0) {
					printf("Error releasing mutex with error %d\n", GetLastError());
				}
				
			}
			else { /* check waitcodes*/
				printf("waitcode received: %d, error: %d\n", file_wait_code, GetLastError());

			}
			count_wait_code = WaitForSingleObject(count_mutex, INFINITE);
			if (count_wait_code == WAIT_OBJECT_0) {
				*(thread_param->counter)++;
				//if (*(thread_param->counter) == );

				/* implement Barrier here */

				error_code = ReleaseMutex(count_mutex);
				if (error_code == 0) {
					printf("Error releasing count mutex with error %d\n", GetLastError());
				}
			}
			else { /* check waitcodes*/
				/* count wait code has error*/
			}
		}
		else {
			/* guest must wait - TIMEOUT */
			printf("Guest %s has to wait for room %s\n", thread_param->guest->name, thread_param->guest->suitable_room);
			break; //this is temporary
		}
	}
	

	/* if waitcode is successful:
	
	
	if waitcode timeout:
	*/
}

WriteToRoomLog(char *guest_name, char *room_name, FILE *fp, int start_day) {
	int roomlog_file_error = 0;
	printf("The guest %s entered room %s\n", guest_name, room_name);
	roomlog_file_error = fopen_s(&fp, "ex03_input\\roomLog.txt", "a");
	if (roomlog_file_error) {
		printf("error opening roomlog file");
	}
	fprintf(fp, "%s %s IN %d\n", room_name, guest_name, start_day);
	fclose(fp);
}
