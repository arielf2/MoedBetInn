#pragma once

#define MAX_NUMBER_OF_GUESTS 15
#define ROOMS_FILE_NAME 13
#define ROOM_GUEST_NAME_LEN 21
#define MAX_LINE_LEN 51
#define MAX_ROOMS 5

typedef struct _room
{
	char name[ROOM_GUEST_NAME_LEN];
	int price_for_night;
	int max_guests;
} room;


void CreateRoom_UpdateArray(char str[], room *room_array[], int index);
void RemoveNewLine(char* str);
void UpdateArray(room *room_array[], char room_name[], int max_guests, int price, int index);