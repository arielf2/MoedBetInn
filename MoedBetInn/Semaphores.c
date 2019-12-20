#include "Semaphores.h"

void CreateRoomSemaphores(HANDLE semaphore_array[], room *room_array[], int num_of_rooms) {
	
	/* iterate throuh the rooms array, get the name of each room and the number of guests it can hold
	   Create a semaphore for each room with the room name, and initialize it to the number of guests*/

	/* Check if the semaphore array is actually needed. the semaphores can be opened using their names, so maybe we don't need to keep their handles?*/

	int i = 0;
	
	for (i; i < num_of_rooms; i++) {
		if (CreateSemaphore(NULL, room_array[i]->max_guests, 10, room_array[i]->name) == NULL){
			printf("Error in create semaphore");
			/* close handles, exit*/
		}
	}
}