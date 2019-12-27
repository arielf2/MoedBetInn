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
	guest *guests_array[MAX_NUMBER_OF_GUESTS];
	room *rooms_array[MAX_ROOMS];
	thread_param_struct* thread_param_array[MAX_NUMBER_OF_GUESTS];
	int num_of_guests = 0;
	int num_of_rooms = 0;
	int day_counter = 0;
	int handles_exit_code;
	int i;
	//char path[] = argv[1] + rooms.txt
	char *rooms_path = NULL;
	char *names_path = NULL;
	char line[MAX_LINE_LEN] = "";
	int rooms_names_len = strlen(argv[1]) + ROOMS_NAMES_FILE_NAME;
	
	// 	allocate memory for full paths of input files, and copy the path to rooms_path/names_path variables
	AllocateMemory_AssignFilename(&rooms_path, argv[1], rooms_names_len, "\\rooms.txt");
	AllocateMemory_AssignFilename(&names_path, argv[1], rooms_names_len, "\\names.txt");

	//Get rooms from input file (this will update rooms_array)
	num_of_rooms = GetRoomsFromFile(rooms_path, rooms_array);
	free(rooms_path); // the filename is no longer needed, free

	//Get guests from input file (this will update guests_array)
	num_of_guests = GetNamesFromFile(names_path, guests_array);
	free(names_path); //the filename is no longer needed, free

	CreateRoomSemaphores(semaphoreHandles, rooms_array, num_of_rooms); 

	int day = 1;
	int counter = 0;
	for (int i = 0; i < num_of_guests; i++) {  /* names_index will hold the actual number of guests*/
		/* find room for guest i*/
		FindRoom_UpdateGuest(guests_array[i], rooms_array, num_of_rooms);  /* room index holds the number of rooms (4 in this case) */
		//*guests_array[i])
		CreateThreadParams(thread_param_array, guests_array, i, &day, &counter, &num_of_guests);
		guest_thread_handles[i] = NULL;

		guest_thread_handles[i] = CreateThreadSimple(GuestThread, (thread_param_array[i]), &(guest_thread_ids[i]));
		if (guest_thread_handles[i] == NULL)
		{
			printf("Couldn't create thread, error code %d\n", GetLastError());
		}
	}

	int waitcode = WaitForMultipleObjects(num_of_guests, guest_thread_handles, TRUE, INFINITE);
	if (waitcode == 0) {
		printf("OK\n");
	}
	// Close thread handles
	for (i = 0; i < num_of_guests; i++)
	{
		handles_exit_code = CloseHandle(guest_thread_handles[i]);
		if (handles_exit_code == 0)
		{
			printf("Error when closing\n");
			
		}
	}
	 

	printf("Number of days until everyone left, %d", day_counter);

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

void CreateThreadParams(thread_param_struct* thread_param_array[], guest* guests_array[], int names_index, int *day, int *counter, int *num_of_guests) {
	thread_param_struct *s_ptr = (thread_param_struct *)malloc(sizeof(thread_param_struct));
	if (NULL == s_ptr) {
		printf("Memory allocation error");
		/* error handle*/
		exit(1);
	}
	s_ptr->guest = guests_array[names_index];
	s_ptr->day = day;
	s_ptr->counter = counter;
	s_ptr->num_of_guests = num_of_guests;
	thread_param_array[names_index] = s_ptr;
}

void AllocateMemory_AssignFilename(char** destination, char* input_folder ,int rooms_names_len, char* filename){ //"\\rooms") {
	*destination = (char *)malloc(rooms_names_len);
	if (destination == NULL) {
		printf("Memory allocation error");
		/* error handle*/
		exit(1);
	}
	strcpy_s(*destination, rooms_names_len, input_folder);
	strcat_s(*destination, rooms_names_len, filename);
	
}

int GetRoomsFromFile(char* rooms_file_path, room *rooms_array[]) {
	
	FILE *rooms_fp = NULL;
	int file_error = 0;
	char line[MAX_LINE_LEN] = "";
	int room_index = 0;
	file_error = fopen_s(&rooms_fp, rooms_file_path, "r");
	if (file_error) {
		printf("error opening file in path %s", rooms_file_path);
	}

	while (feof(rooms_fp) == 0) {
		///////////////reset line
		fgets(line, MAX_LINE_LEN, rooms_fp);
		RemoveNewLine(line);
		CreateRoom_UpdateArray(line, rooms_array, room_index);
		room_index++;
	}

	return room_index; /* this will be the actual number of available rooms */
}

int GetNamesFromFile(char* names_file_path, guest *guests_array[]) {

	FILE *names_fp = NULL;
	int file_error = 0;
	char line[MAX_LINE_LEN] = "";
	int names_index = 0;
	file_error = fopen_s(&names_fp, names_file_path, "r");
	if (file_error) {
		printf("error opening file in path %s", names_file_path);
	}

	while (feof(names_fp) == 0) {
		/////reset line
		fgets(line, MAX_LINE_LEN, names_fp);
		RemoveNewLine(line);
		CreateGuests_UpdateArray(line, guests_array, names_index);
		// CreateThreadParams(thread_param_array, guests_array, names_index); // moved this line to the for loop below
		names_index++;
	}

	return names_index; /* this will be the actual number of guests */
}