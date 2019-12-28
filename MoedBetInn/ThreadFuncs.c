#include "ThreadFuncs.h"
#include "Header.h"


extern HANDLE barrier_semaphore;
extern HANDLE second_door_semaphore;
extern HANDLE log_file_mutex;
extern HANDLE count_mutex;
extern HANDLE check_leaving_mutex;
extern int start_days[MAX_NUMBER_OF_GUESTS];
extern int guests_currently_in_rooms;
extern int guests_waiting_for_rooms;
//int CreateThreadsForGuests(guest *guests_array[]) {
//
//}

void GuestEnterRoom(thread_param_struct *thread_param, int *start_day, int index);

int CheckLeaveRoom(thread_param_struct *thread_param, FILE *roomlog_fp, int* guest_left, int* start_day, HANDLE *room_semaphore);

void UpdateCounter_GotoBarrier(thread_param_struct *thread_param, HANDLE curr_handle, int *guests_should_leave);

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
	 //CreateSemaphore(NULL, thread_param->, 10, room_array[i]->name)
	 //HANDLE room_semaphore = OpenSemaphore(SYNCHRONIZE, FALSE, thread_param->guest->suitable_room);
	HANDLE room_semaphore = CreateSemaphore(NULL, *(thread_param->max_guests_in_suitable_room), MAX_NUMBER_OF_GUESTS, thread_param->guest->suitable_room);
	HANDLE personal_semaphore = CreateSemaphore(NULL, 0, 1, thread_param->guests[thread_param->index]);
	HANDLE leaving_guests_semaphore = CreateSemaphore(NULL, 0, MAX_NUMBER_OF_GUESTS, "leaving_guests_semaphore");


	//barrier_semaphore = OpenSemaphore()
	//HANDLE barrier_semaphore = NULL;
	//HANDLE log_file_mutex = NULL;
	//HANDLE count_mutex = NULL;

	BOOL   barrier_retrun_value;
	BOOL   count_mutex_retrun_value;
	BOOL   room_semaphore_return_value;
	BOOL   others_semaphore_return_value;
	int   leaving_guests_wait_code;
	int start_day = 0;
	int room_wait_code = 0;
	int count_wait_code = 0;
	int file_wait_code = 0;
	int barrier_wait_code = 0;
	int personal_semaphore_wait_code = 0;
	int roomlog_file_error = 0;
	FILE *roomlog_fp;
	int count_error_code = 0;
	int second_door_waitcode = 0;
	int check_leaving_waitcode = 0;
	int log_file_error_code;
	int guest_left = 0;
	int leave = 0;
	int in_room = 0;
	int open_barrier_day = 0;
	guest *guests_array[MAX_NUMBER_OF_GUESTS];
	HANDLE curr_handle = NULL;
	int guests_should_leave = 0;
	//int start_days[MAX_NUMBER_OF_GUESTS];
	//start_days[0] = (thread_param->start_days);
	
	guests_array[0] = *(thread_param->guests);

	while (1) {
			if (guest_left == 1)
			break;
		//printf("Guest %s trying to enter room %s, checking room semaphore\n", thread_param->guest->name, thread_param->guest->suitable_room);

		room_wait_code = WaitForSingleObject(room_semaphore, TIMEOUT);
		// check waitcodes
		if (room_wait_code == WAIT_OBJECT_0) {	/* guest can enter the room */
			printf("The guest %s entered room %s\n", thread_param->guest->name,thread_param->guest->suitable_room);

			while (1) {

				//count_mutex = OpenMutex(SYNCHRONIZE, FALSE, "countMutex");
				if (in_room == 0) {
					// guest now entered room
					start_days[thread_param->index] = *(thread_param->day);
					GuestEnterRoom(thread_param, &start_day, (thread_param->index));
					in_room = 1;
					guests_currently_in_rooms = guests_currently_in_rooms + 1;

				}

				//printf("Guest %s now waiting on count mutex\n", thread_param->guest->name);
				count_wait_code = WaitForSingleObject(count_mutex, INFINITE);
				if (count_wait_code == WAIT_OBJECT_0) {
					UpdateCounter_GotoBarrier(thread_param, curr_handle, &guests_should_leave, &start_days);


					count_error_code = ReleaseMutex(count_mutex);
					if (count_error_code == 0) {
						printf("Error releasing count mutex with error %d\n", GetLastError());
					}
					/* personal semaphore wait*/
					//personal_semaphore_wait_code = WaitForSingleObject(personal_semaphore, INFINITE);
					if (personal_semaphore_wait_code == WAIT_OBJECT_0) {
						barrier_wait_code = WaitForSingleObject(barrier_semaphore, INFINITE);
						if (barrier_wait_code == WAIT_OBJECT_0) {
							printf("Guest %s pass barrier\n", thread_param->guest->name);
							/* check day and see if should leave room*/
							if (CheckLeaveRoom(thread_param, &roomlog_fp, &guest_left, &start_day, &room_semaphore) == 1) {
								guest_left = 1;
								guests_currently_in_rooms = guests_currently_in_rooms - 1;
								return 0;
							}
							//barrier_semaphore = NULL;
						}
						else {
							/* check waitcodes*/
							/* barrier wait code has error*/
							printf("barrier error %d, code %d\n", barrier_wait_code, GetLastError());
							//barrier_semaphore = NULL;
						}
					}
				}
				else if (count_wait_code != WAIT_OBJECT_0) { // count wait code != 0
					printf("error in count wait, error code %d\n", GetLastError());
				}

			}
		}
		else {
			/* guest must wait - TIMEOUT */
			printf("Guest %s has to wait for room %s\n", thread_param->guest->name, thread_param->guest->suitable_room);
			//count_mutex = OpenMutex(SYNCHRONIZE, FALSE, "countMutex");

			count_wait_code = WaitForSingleObject(count_mutex, INFINITE);
			if (count_wait_code == WAIT_OBJECT_0) {
				/* implement Barrier here */


				UpdateCounter_GotoBarrier(thread_param, curr_handle, &guests_should_leave, &start_days);

				count_error_code = ReleaseMutex(count_mutex);
				if (count_error_code == 0) {
					printf("Error releasing count mutex with error %d\n", GetLastError());
				}
				/* personal_semaphore_wait */
				personal_semaphore_wait_code = WaitForSingleObject(personal_semaphore, INFINITE);
				if (personal_semaphore_wait_code == WAIT_OBJECT_0) {
					/*barrier wait*/
					//barrier_semaphore = OpenSemaphore(SYNCHRONIZE, FALSE, "barrierSemaphore");
					barrier_wait_code = WaitForSingleObject(barrier_semaphore, INFINITE);
					if (barrier_wait_code == WAIT_OBJECT_0) {
						/*do nothing*/
						//barrier_semaphore = NULL;
						printf("Guest %s pass barrier\n", thread_param->guest->name);

						/* wait for guests should leave semaphore*/
						//l_handle = CreateSemaphore(NULL, 0, MAX_NUMBER_OF_GUESTS, "leaving_guests_semaphore");

						check_leaving_waitcode = WaitForSingleObject(check_leaving_mutex, INFINITE);
						/*mutex down */
						guests_waiting_for_rooms = guests_waiting_for_rooms + 1;   /* THIS VARIABLE NEEDS TO BE DECREMENTED SOMEWHERE, NOT SURE WHERE*/
						int waiting_threshold = thread_param->num_of_guests - guests_should_leave - guests_currently_in_rooms;

						if (check_leaving_waitcode == WAIT_OBJECT_0) {
							for (int m = 0; m < guests_should_leave; m++) {
								leaving_guests_wait_code = WaitForSingleObject(leaving_guests_semaphore, INFINITE);
								if (leaving_guests_wait_code != WAIT_OBJECT_0)
									printf("Error while waiting leaving guests semaphore\n");
								leaving_guests_wait_code = -1;
							}
							if (guests_waiting_for_rooms != waiting_threshold) { // all the guests except the last waiting guest will release the semaphore
								int release_code = ReleaseSemaphore(leaving_guests_semaphore, guests_should_leave, NULL);
								if (release_code == 0)
									printf("error releasing leaving guests semaphore with error code %d", GetLastError());
							}
						}
						ReleaseMutex(check_leaving_mutex);
						

						/* כולם חוץ מהאחרון עושים ריליס, כדי שהסמפור יישאר על 0 ביום חדש*/
						// num_of_guests - guests_should_leave - guests_currently_in_rooms
						/* release leaving_guests_semaphore(with leaving guests)  */
						/* mutex up */
					}
					else {
						/* check waitcodes*/
					/* barrier wait code has error*/
						printf("barrier error with error %d\n", GetLastError());
						//barrier_semaphore = NULL;

					}
				}
				else { /* check waitcodes*/
					/* count wait code has error*/
					printf("count error");
				}
			}
			//break; //this is temporary
		}
	}


	/* if waitcode is successful:


	if waitcode timeout:
	*/
}




WriteToRoomLogIn(char *guest_name, char *room_name, FILE *fp, int start_day) {
	int roomlog_file_error = 0;
	roomlog_file_error = fopen_s(&fp, "ex03_input\\roomLog.txt", "a");
	if (roomlog_file_error) {
		printf("error opening roomlog file");
	}
	fprintf(fp, "%s %s IN %d\n", room_name, guest_name, start_day);
	fclose(fp);
}

WriteToRoomLogOut(char *guest_name, char *room_name, FILE *fp, int leave_day) {
	int roomlog_file_error = 0;
	printf("The guest %s left room %s\n", guest_name, room_name);
	roomlog_file_error = fopen_s(&fp, "ex03_input\\roomLog.txt", "a");
	if (roomlog_file_error) {
		printf("error opening roomlog file");
	}
	fprintf(fp, "%s %s OUT %d\n", room_name, guest_name, leave_day);
	fclose(fp);
}

void GuestEnterRoom(thread_param_struct *thread_param, int *start_day, int index) {

	FILE *roomlog_fp;
	int log_file_error_code = 0;
	int file_wait_code = 0;
	
	// guest now entered room
	/* GuestEnterRoom */
	*(start_day) = *(thread_param->day);
	//HANDLE log_file_mutex = OpenMutex(SYNCHRONIZE, FALSE, "logFileMutex");
	//printf("Guest %s now waiting for log in mutex\n", thread_param->guest->name);
	file_wait_code = WaitForSingleObject(log_file_mutex, INFINITE);
	if (file_wait_code == WAIT_OBJECT_0) {
		printf("Guest %s now writing log in file\n", thread_param->guest->name);
		/* means no one is writing the file, the mutex is "open" for this thread to write */
		/* write to log file */
		WriteToRoomLogIn(thread_param->guest->name, thread_param->guest->suitable_room, &roomlog_fp, *(start_day));
		log_file_error_code = ReleaseMutex(log_file_mutex);

		if (log_file_error_code == 0) {
			printf("Error releasing mutex with error %d\n", GetLastError());
		}
	}
	else { /* check waitcodes*/
		printf("waitcode received: %d, error: %d\n", file_wait_code, GetLastError());
	}
	/*Update global start days*/
	
	
}

void UpdateCounter_GotoBarrier(thread_param_struct *thread_param, HANDLE curr_handle, int* guests_should_leave) {
	int barrier_return_value = 0;
	int i = 0, j = 0;
	HANDLE l_handle;
	*(thread_param->counter) = *(thread_param->counter) + 1;
	printf("Current day: %d, guest %s increased counter to: %d\n", *thread_param->day, thread_param->guest->name, *thread_param->counter);

	//*(thread_param->counter)++;
	if (*(thread_param->counter) == *(thread_param->num_of_guests)) {
		
		*(thread_param->day) = *(thread_param->day) + 1;
		printf("guest %s increase day to: %d\n", thread_param->guest->name, *(thread_param->day));
		/* temp */
		*(thread_param->counter) = 0;

		/* Check who should leave today*/
		for (j = 0; j < MAX_NUMBER_OF_GUESTS; j++) {
			if (start_days[j] != -1)
				if (thread_param->num_of_nights[j] <= (*(thread_param->day) - start_days[j]))
					*guests_should_leave = *guests_should_leave + 1;
		}
		

		printf("day passed to day %d\n", *(thread_param->day));
		barrier_return_value = ReleaseSemaphore(barrier_semaphore, *(thread_param->num_of_guests), NULL);
		if (barrier_return_value == 0) {
			printf("Error releasing barrier semaphore with error %d\n", GetLastError());
		}

		/*Relese everyone's semaphore*/
		for (i = 0; i < *(thread_param->num_of_guests); i++) {
			curr_handle = CreateSemaphore(NULL, 0, 1, (thread_param->guests[i]));
			ReleaseSemaphore(curr_handle, 1, NULL);
			curr_handle = NULL;
		}



		/*                   */

		/* else if (second_door_waitcode)  == error..
		*/
		//printf("Day updates to %d by %s\n", *(thread_param->day), *(thread_param->guest->name));
		//barrier_semaphore = OpenSemaphoreW(SYNCHRONIZE, FALSE, "barrierSemaphore");

		//barrier_semaphore = NULL;
	}
}

int CheckLeaveRoom(thread_param_struct *thread_param, FILE *roomlog_fp, int* guest_left, int* start_day, HANDLE *room_semaphore) {
	int log_file_error_code = 0;
	int room_semaphore_return_value = 0;
	int leaving_guests_return_value = 0;
	int file_wait_code = 0;
	int leave = 0;
	HANDLE l_handle;

	if ((thread_param->guest->num_of_nights) <= (*(thread_param->day) - *(start_day))) {/*means guest should leave*/
		file_wait_code = WaitForSingleObject(log_file_mutex, INFINITE);
		if (file_wait_code == WAIT_OBJECT_0) {
			/* means no one is writing the file, the mutex is "open" for this thread to write */
			/* write to log file */
			WriteToRoomLogOut(thread_param->guest->name, thread_param->guest->suitable_room, &roomlog_fp, *(thread_param->day));
			log_file_error_code = ReleaseMutex(log_file_mutex);
			if (log_file_error_code == 0) {
				printf("Error releasing mutex with error %d\n", GetLastError());
			}
			/*release room semaphore*/
			room_semaphore_return_value = ReleaseSemaphore(*(room_semaphore), 1, NULL);
			if (room_semaphore_return_value == 0) {
				printf("Error releasing room semaphore with error %d\n", GetLastError());
			}
			*(guest_left) = 1;
			//break; // return 0 maybe to break from 2 while loops?
			leave = 1;
			*(thread_param->num_of_guests) = *(thread_param->num_of_guests) - 1;
			/* update global start day to -1 means left*/

			//*thread_param->start_days[thread_param->index] = -1;
			start_days[thread_param->index] = -1;
			printf("Guest %s has to leave room: %s\n", thread_param->guest->name, thread_param->guest->suitable_room);
			/* Release leaving guests semaphore*/
			l_handle = CreateSemaphore(NULL, 0, MAX_NUMBER_OF_GUESTS, "leaving_guests_semaphore");
			leaving_guests_return_value = ReleaseSemaphore(l_handle, 1, NULL);
			if (leaving_guests_return_value == 0) {
				printf("Error releasing leaving guests semaphore with error %d\n", GetLastError());
			}

		}

		else { /* check waitcodes*/
			printf("waitcode received: %d, error: %d\n", file_wait_code, GetLastError());

		}
	}

	return leave;


}