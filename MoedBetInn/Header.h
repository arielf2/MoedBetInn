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
	guest guest;
	int *day;

} thread_param_struct;

void CreateRoom_UpdateArray(char str[], room *room_array[], int index);
void CreateGuests_UpdateArray(char str[], guest* names_array[], int index);
void RemoveNewLine(char* str);
void UpdateArrayRooms(room *room_array[], char room_name[], int max_guests, int price, int index);
void UpdateArrayNames(guest *names_array[], char name[], int nights, int index);
FindRoom_UpdateGuest(guest *guest_to_check, room *room_array[], int num_of_rooms);