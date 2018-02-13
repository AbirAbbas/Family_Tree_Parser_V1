#include <stdio.h>

#include "LinkedListAPI.h"
#include "GEDCOMparser.h"
#include "GEDCOMutilities.h"

int main () {
	
	GEDCOMobject * obj = NULL;
	
	GEDCOMerror e = createGEDCOM("shakespeare.ged", &(obj));
	
	if (e.type == OK) {
		printf("No errors reported\n");
	}
	else {
		printf("Error!\n");
	}
	
	return 0;
}
