#pragma once
#pragma warning(disable:4996)

#include <stdlib.h>
#include <stdio.h>
#include <windows.h>

#define MAX_NUMBER_OF_GUESTS 15
#define ROOMS_NAMES_FILE_NAME 13
#define ROOM_GUEST_NAME_LEN 21
#define MAX_LINE_LEN 51
#define MAX_ROOMS 5

typedef struct _room
{
	char name[ROOM_GUEST_NAME_LEN];
	int price_for_night;
	int max_guests;
} room;

typedef struct _guest
{
	char name[ROOM_GUEST_NAME_LEN];
	int money;

	char suitable_room[ROOM_GUEST_NAME_LEN];
	int num_of_nights;

} guest;

typedef struct _thread_param_struct
{
	guest *guest;

	int *day;
	int *counter;
	int *num_of_guests;
	char guests[MAX_NUMBER_OF_GUESTS][ROOM_GUEST_NAME_LEN];
	int* start_days[MAX_NUMBER_OF_GUESTS];
	int index;
	int *max_guests_in_suitable_room;
} thread_param_struct;


/*	Name: CreateRoom_UpdateArray
	Description:
	Parameters:
	Returns:	*/
void CreateRoom_UpdateArray(char str[], room *room_array[], int index);

/*	Name: CreateGuests_UpdateArray
	Description:
	Parameters:
	Returns:	*/
void CreateGuests_UpdateArray(char str[], guest* names_array[], int index);

/*	Name: RemoveNewLine
	Description:
	Parameters:
	Returns:	*/
void RemoveNewLine(char* str);

/*	Name: UpdateArrayRooms
	Description:
	Parameters:
	Returns:	*/
void UpdateArrayRooms(room *room_array[], char room_name[], int max_guests, int price, int index);

/*	Name: UpdateArrayNames
	Description:
	Parameters:
	Returns:	*/
void UpdateArrayNames(guest *names_array[], char name[], int nights, int index);

/*	Name: FindRoom_UpdateGuest
	Description:
	Parameters:
	Returns:	*/
int FindRoom_UpdateGuest(guest *guest_to_check, room *room_array[], int num_of_rooms);

/*	Name: CreateThreadParams
	Description:
	Parameters:
	Returns:	*/
void CreateThreadParams(thread_param_struct* thread_param_array[], guest* guests_array[], int names_index, int* day, int* counter, int *num_of_guests, int* max_guests);

/*	Name: AllocateMemory_AssignFilename
	Description:
	Parameters:
	Returns:	*/
void AllocateMemory_AssignFilename(char** destination, char* input_folder, int rooms_names_len, char* filename);

/*	Name: GetRoomsFromFile
	Description:
	Parameters:
	Returns:	*/
int GetRoomsFromFile(char* rooms_file_path, room *rooms_array[]);

/*	Name: GetNamesFromFile
	Description:
	Parameters:
	Returns:	*/
int GetNamesFromFile(char* names_file_path, guest *guests_array[]);

/*	Name: CreateRoom_UpdateArray
Description:
Parameters:
Returns:	*/
int CloseHandles(HANDLE thread_handles[], int num_of_threads);