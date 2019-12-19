/* Authors: Rotem Hecht 311143044
			Ariel Felberg 308425974
   Project: ISP Exercise 3
   Description:
*/

#include <stdlib.h>
#include <stdio.h>
#include <windows.h>
#include "Header.h"







int main(int argc, char *argv[]) {

	char delim = " ";
	HANDLE countMutex = CreateMutex(NULL, FALSE, "countMutex");
	HANDLE logFileMutex = CreateMutex(NULL, FALSE, "logFileMutex");
	HANDLE barrierSemaphore = CreateSemaphore(NULL, 0, MAX_NUMBER_OF_GUESTS, "barrierSemaphore");
	FILE *rooms_fp = NULL;
	FILE *names_fp = NULL;
	guest *guests_array[MAX_NUMBER_OF_GUESTS];
	room *rooms_array[MAX_ROOMS];
	int room_index = 0;
	int names_index = 0;
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
	file_error = fopen_s(&fp, rooms_path, "r");
	if (file_error) {
		printf("error opening file in path %s", rooms_path);
	}

	while (feof(rooms_fp) == 0) {
		///////////////reset line
		fgets(line, MAX_LINE_LEN, rooms_fp);
		RemoveNewLine(&line);
		CreateRoom_UpdateArray(line, rooms_array, room_index);
		/* check that the array is updated with the correct room */
		printf("%s %d %d\n", rooms_array[room_index]->name, rooms_array[room_index]->price_for_night, rooms_array[room_index]->max_guests);
		room_index++;	
	}
	getchar();///???????

	//Get names from file
	file_error = fopen_s(&fp, names_path, "r");
	if (file_error) {
		printf("error opening file in path %s", names_path);
	}

	while (feof(names_fp) == 0) {
		/////reset line
		fgets(line, MAX_LINE_LEN, names_fp);
		RemoveNewLine(&line);
		CreateGuests_UpdateArray(line, names_array, names_index);
		names_index++;
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

void CreateGuestsUpdateArray(char str[], guest* names_array[], int index) {
	char delim[2] = " ";
	char* name;
	char* nigths;
	int nights_i;

	name = strtok(str, delim);
	nights = strtok(str, delim);
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
		exit;
	}
	r_ptr->max_guests = max_guests;
	r_ptr->price_for_night = price;
	strcpy_s(r_ptr->name, ROOM_GUEST_NAME_LEN, room_name);
	room_array[index] = r_ptr;
}

void UpdateArrayNames(guest *names_array[], char name[], int nights, int index) {
	guest *n_ptr = (room *)malloc(sizeof(guest));
	if (NULL == n_ptr) {
		printf("Memory allocation error");
		/* error handle*/
		exit;
	}
	n_ptr->nigths = nights;
	strcpy_s(n_ptr->name, ROOM_GUEST_NAME_LEN, name);
	names_array[index] = n_ptr;
}