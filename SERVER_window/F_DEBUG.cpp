#include "_DEBUG.h"


void DEBUG_ListShow(list<ROOM> List) {
	list<ROOM>::iterator ptr;

	for (ptr = List.begin(); ptr != List.end(); ptr++) {
		ptr->Show();

	}
}