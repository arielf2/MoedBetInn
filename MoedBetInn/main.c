/* Authors: Ariel Felberg 308425974
			Rotem Hecht   311143044
   Project: ISP Exercise 3
   Description:
*/


#include "Header.h"
#include "ThreadFuncs.h"
#include "Semaphores.h"



int main(int argc, char *argv[]) {

	char delim = " ";
	HANDLE countMutex = CreateMutex(NULL, FALSE, "countMutex");
	HANDLE logFileMutex = CreateMutex(NULL, FALSE, "logFileMutex");
	HANDLE barrierSemaphore = CreateSemaphore(NULL, 0, MAX_NUMBER_OF_GUESTS, "barrierSemaphore");
	HANDLE guest_thread_handles[MAX_NUMBER_OF_GUESTS];
	int guest_thread_ids[MAX_NUMBER_OF_GUESTS];

	HANDLE semaphoreHandles[MAX_ROOMS];
	FILE *rooms_fp = NULL;
	FILE *names_fp = NULL;
	guest *guests_array[MAX_NUMBER_OF_GUESTS];
	room *rooms_array[MAX_ROOMS];
	thread_param_struct* thread_param_array[MAX_NUMBER_OF_GUESTS];
	int room_index = 0;
	int names_index = 0;
	int day_counter = 0;
	//char path[] = argv[1] + rooms.txt
	char *rooms_path;
	char *names_path;
	char line[MAX_LINE_LEN] = "";

	//create rooms file path
	int rooms_names_len = strlen(argv[1]) + ROOMS_NAMES_FILE_NAME;
	rooms_path = (char *)malloc(rooms_names_len);
	if (rooms_path == NULL) {
		printf("Memory allocation error");
		/* error handle*/
		exit(1);
	}
	strcpy_s(rooms_path, rooms_names_len, argv[1]);
	strcat_s(rooms_path, rooms_names_len, "\\rooms.txt");

	//create names file path
	names_path = (char *)malloc(rooms_names_len);
	if (names_path == NULL) {
		printf("Memory allocation error");
		/* error handle*/
		exit(1);
	}
	strcpy_s(names_path, rooms_names_len, argv[1]);
	strcat_s(names_path, rooms_names_len, "\\names.txt");

	//Get rooms from file
	int file_error;
	file_error = fopen_s(&rooms_fp, rooms_path, "r");
	if (file_error) {
		printf("error opening file in path %s", rooms_path);
	}

	while (feof(rooms_fp) == 0) {
		///////////////reset line
		fgets(line, MAX_LINE_LEN, rooms_fp);
		RemoveNewLine(line);
		CreateRoom_UpdateArray(line, rooms_array, room_index);
		room_index++;	
	}
	CreateRoomSemaphores(semaphoreHandles, rooms_array, room_index); 

	//The following is just an example to check how open semaphore works. We maybe don't need the semaphore handles array

	//HANDLE semHan = OpenSemaphore(SYNCHRONIZE, FALSE, "RoomA");
	//int waitcode = WaitForSingleObject(semHan, 5000);
	//printf("\nreturned wait code in 1: %d\n", waitcode);
	//waitcode = WaitForSingleObject(semHan, 5000);
	//printf("returned wait code in 2: %d\n", waitcode);
	//waitcode = WaitForSingleObject(semHan, 5000);
	//printf("returned wait code in 3: %d\n", waitcode);
	//waitcode = WaitForSingleObject(semHan, 5000);
	//printf("returned wait code in 4: %d\n", waitcode);


	//Get names from file
	file_error = fopen_s(&names_fp, names_path, "r");
	if (file_error) {
		printf("error opening file in path %s", names_path);
	}

	while (feof(names_fp) == 0) {
		/////reset line
		fgets(line, MAX_LINE_LEN, names_fp);
		RemoveNewLine(line);
		CreateGuests_UpdateArray(line, guests_array, names_index);
		// CreateThreadParams(thread_param_array, guests_array, names_index); // moved this line to the for loop below
		names_index++;
	}
	int day = 1;
	int counter = 0;
	for (int i = 0; i < names_index; i++) {  /* names_index will hold the actual number of guests*/
		/* find room for guest i*/
		FindRoom_UpdateGuest(guests_array[i], rooms_array, room_index);  /* room index holds the number of rooms (4 in this case) */
		//*guests_array[i])
		CreateThreadParams(thread_param_array, guests_array, i, &day, &counter);
		guest_thread_handles[i] = NULL;

		guest_thread_handles[i] = CreateThreadSimple(GuestThread, (thread_param_array[i]), &(guest_thread_ids[i]));
		if (guest_thread_handles[i] == NULL)
		{
			printf("Couldn't create thread, error code %d\n", GetLastError());
		}
	}

	int waitcode = WaitForMultipleObjects(names_index, guest_thread_handles, TRUE, INFINITE);
	if (waitcode == 0) {
		printf("OK\n");
	}



}

void CreateRoom_UpdateArray(char str[], room *room_array[], int index) {
	/* gets the line as input, initialize struct and read the line to get its fields
	   return the struct */
	int i = 0, max_guests = 0, price_for_night = 0, start_index = 0;
	char price_str[10], guests_str[10];
	char room_name[ROOM_GUEST_NAME_LEN];
	/* get room_name */
	for (i; str[i] != ' '; i++) {
		room_name[i] = str[i];
	}
	room_name[i] = '\0';
	i++;
	start_index = i;
	/* get price */
	for (i; str[i] != ' '; i++){
		price_str[i - start_index] = str[i];
	}
	price_str[i - start_index] = '\0';
	i++;
	start_index = i;
	/* get max guests */
	for (i; str[i] != '\0'; i++) {
		guests_str[i - start_index] = str[i];
	}
	guests_str[i - start_index] = '\0';
	sscanf_s(price_str, "%d", &price_for_night);
	sscanf_s(guests_str, "%d", &max_guests);


	UpdateArrayRooms(room_array, room_name, max_guests, price_for_night, index);
	
}

void CreateGuests_UpdateArray(char str[], guest* names_array[], int index) {
	char delim[2] = " ";
	char* name;
	char* nights;
	int nights_i;

	name = strtok(str, delim);
	nights = strtok(NULL, delim);
	nights_i = atoi(nights);
	
	UpdateArrayNames(names_array, name, nights_i, index);

}

void RemoveNewLine(char* str) {
	int len = strlen(str);
	if (str[len - 1] == '\n') {
		str[len - 1] = '\0';
	}
	
}

void UpdateArrayRooms(room *room_array[], char room_name[], int max_guests, int price, int index) {
	room *r_ptr = (room *)malloc(sizeof(room));
	if (NULL == r_ptr) {
		printf("Memory allocation error");
		/* error handle*/
		exit(1);
	}
	r_ptr->max_guests = max_guests;
	r_ptr->price_for_night = price;
	strcpy_s(r_ptr->name, ROOM_GUEST_NAME_LEN, room_name);
	room_array[index] = r_ptr;
}

void UpdateArrayNames(guest *names_array[], char name[], int nights, int index) {
	guest *n_ptr = (guest *)malloc(sizeof(guest));
	if (NULL == n_ptr) {
		printf("Memory allocation error");
		/* error handle*/
		exit(1);
	}
	n_ptr->money = nights;
	strcpy_s(n_ptr->name, ROOM_GUEST_NAME_LEN, name);
	names_array[index] = n_ptr;
}

void FindRoom_UpdateGuest(guest *guest_to_check, room *room_array[], int num_of_rooms) {
	int sum = guest_to_check->money;
	int i = 0;

	for (i; i < num_of_rooms; i++) {
		if (sum % (room_array[i]->price_for_night) == 0) {  /* 46 % 23 == 0 for example*/
			guest_to_check->num_of_nights = (sum / room_array[i]->price_for_night);
			strcpy_s(guest_to_check->suitable_room, ROOM_GUEST_NAME_LEN, room_array[i]->name);
			break;
		}
	}

}


void CreateThreadParams(thread_param_struct* thread_param_array[], guest* guests_array[], int names_index, int *day, int *counter) {
	thread_param_struct *s_ptr = (thread_param_struct *)malloc(sizeof(thread_param_struct));
	if (NULL == s_ptr) {
		printf("Memory allocation error");
		/* error handle*/
		exit(1);
	}
	s_ptr->guest = guests_array[names_index];
	s_ptr->day = day;
	s_ptr->counter = counter;
	thread_param_array[names_index] = s_ptr;
}