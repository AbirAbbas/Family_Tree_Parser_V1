#include <stdio.h>

#include "LinkedListAPI.h"
#include "GEDCOMparser.h"
#include "GEDCOMutilities.h"

int main () {
	
	GEDCOMobject * obj = NULL;
	
	GEDCOMerror e = createGEDCOM("simpleValid1R.ged", &(obj));
	
	printf("%s", printError(e));
	
	return 0;
}
